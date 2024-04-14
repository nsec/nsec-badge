#include <string.h>
#include "esp_random.h"
#include "esp_flash.h"
#include "ota_init.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ota_actions.h"

#define READSIZE (4096 * 1)
#define FLASH_DEST_ADDR 0x400000

static const char *TAG = "ota_actions";

std::string get_firmware_project_name(esp_partition_subtype_t subtype) {
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Failed to find subtype %d partition", subtype);
        return std::string("");
    }
    esp_app_desc_t desc;
    esp_err_t err = esp_ota_get_partition_description(partition, &desc);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to get partition description: %s (0x%x)", esp_err_to_name(err), err);
        return std::string("");
    }
    ESP_LOGD(TAG, "Project name: %s", desc.project_name);
    return std::string(desc.project_name);
}

std::string get_firmware_select_string() {
    std::string return_string = "[";
    std::string project_name;

    if (get_firmware_project_name(NSEC_CONF_PARTITION) == "nsec-badge") {
        return_string += "conf";
    }

    if (get_firmware_project_name(NSEC_CTF_PARTITION) == "nsec-ctf") {
        return_string += "|ctf";
    }
 
    if (get_firmware_project_name(NSEC_OTA_PARTITION) == "nsec-ctf-addon") {
        return_string += "|addon";
    }
 
    return return_string + "]";
}

bool boot_partition(esp_partition_subtype_t subtype) {
    // Set the OTA boot partition
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Failed to find subtype %d partition", subtype);
        return false;
    }

    ESP_LOGI(TAG, "Setting boot partition...");
    
    esp_err_t err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, error=%d", err);
        return false;
    }

    ESP_LOGI(TAG, "Restarting...");
    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_restart();
}

int ota_cmd(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: firmware_select %s\n", get_firmware_select_string().c_str());
        return ESP_OK;
    }
    // get argv into a string
    char* firmware = argv[1];
    if (strcmp(firmware, "conf") == 0) {
        ESP_LOGI(TAG, "Switching to conf firmware...");
        if(!boot_partition(NSEC_CONF_PARTITION)) {
            ESP_LOGE(TAG, "Failed to boot to conf firmware - this should not happen!");
        }
    } else if (strcmp(firmware, "ctf") == 0) {
        ESP_LOGI(TAG, "Switching to ctf firmware...");
        if(!boot_partition(NSEC_CTF_PARTITION)) {
            ESP_LOGE(TAG, "Failed to boot to ctf firmware. You need to use this during the CTF after you go see the admin table to get flashed!");
        }
    } else if (strcmp(firmware, "addon") == 0) {
        ESP_LOGI(TAG, "Switching to ctf addon firmware...");
        if(!boot_partition(NSEC_OTA_PARTITION)) {
            ESP_LOGE(TAG, "Failed to boot to ctf addon firmware. You need to insert the addon given to you during the CTF to get the firmware!");
        }
    } else {
        printf("Usage: firmware_select %s\n", get_firmware_select_string().c_str());
    }
    return ESP_OK;
}

// Read firmware from flash and write it to NSEC_OTA_PARTITION
// @returns true if OK and false if ERROR
bool write_flash_to_ota(esp_flash_t* _flash) {
    // Check if OTA partition already has the firmware
    esp_partition_subtype_t subtype = NSEC_OTA_PARTITION;
    const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (ota_partition != NULL) {
        ESP_LOGI(TAG, "Found OTA partition %d! size: %lu", NSEC_OTA_ID, ota_partition->size);
        esp_app_desc_t desc;
        esp_err_t err = esp_ota_get_partition_description(ota_partition, &desc);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "No ota_%d partition firmware detected: %s (0x%x)", NSEC_OTA_ID, esp_err_to_name(err), err);
        } else {
            ESP_LOGI(TAG, "OTA %d firmware project: %s", NSEC_OTA_ID, desc.project_name);
            ESP_LOGI(TAG, "OTA %d firmware version: %s", NSEC_OTA_ID, desc.version);
        }
        if (strcmp(desc.project_name, "nsec-ctf-addon") == 0) {
            ESP_LOGI(TAG, "OTA %d already populated with nsec-ctf-addon firmware, skipping OTA write", NSEC_OTA_ID);
            return true;
        }
    } else {
        ESP_LOGE(TAG, "Failed to find ota_%d partition. Most likely due to wrong firmware loaded.", NSEC_OTA_ID);
        return false;
    }
    ESP_LOGI(TAG, "Writing to OTA partition %d from external flash...", NSEC_OTA_ID);

    esp_ota_handle_t ota_handle;

    // Begin an OTA operation
    esp_err_t err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        return false;
    }

    // Buffer for data
    uint8_t* buffer = (uint8_t*)malloc(READSIZE);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return false;
    }

    int toggle = 0;
    for (size_t offset = 0; offset < ota_partition->size; offset += READSIZE) {
        // Copy data to buffer
        ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, FLASH_DEST_ADDR + offset, READSIZE));

        // Write data to OTA partition
        err = esp_ota_write(ota_handle, buffer, READSIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed, error=%d", err);
            return false;
        }

        memset(buffer, 0, READSIZE);
        gpio_set_level(ADDON_BLUE_LED, toggle);
        toggle = !toggle;
    }

    // End the OTA operation
    err = esp_ota_end(ota_handle);
    free(buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed, error=%d", err);
        return false;
    }
    
    // Set the OTA boot partition and restart
    return boot_partition(ota_partition->subtype);
}

#if CTF_ADDON_ADMIN_MODE
// Read data from an OTA partition and write it to flash
void storage_read_from_ota(int ota, esp_flash_t* _flash) {
    // Get the OTA partition
    esp_partition_subtype_t subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
    if (ota == 1) {
        subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
    } else if(ota != 0) {
        ESP_LOGE(TAG, "Invalid OTA partition number: %d", ota);
        return;
    }

    const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (ota_partition == NULL) {
        ESP_LOGE(TAG, "Failed to get OTA partition!");
        return;
    }
    size_t size = ota_partition->size;
    ESP_LOGI(TAG, "Found OTA partition %d! size: %d", ota, size);

    // Buffer for data
    uint8_t* buffer = (uint8_t*)malloc(READSIZE);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return;
    }

    // Read data from OTA partition
    ESP_ERROR_CHECK(esp_partition_read(ota_partition, 0, buffer, READSIZE));

    int toggle = 0;

    for (size_t offset = 0; offset < size; offset += READSIZE) {
        // Read data from OTA partition
        ESP_ERROR_CHECK(esp_partition_read(ota_partition, offset, buffer, READSIZE));

        // Write data to flash
        ESP_ERROR_CHECK(esp_flash_erase_region(_flash, FLASH_DEST_ADDR + offset, READSIZE));
        ESP_ERROR_CHECK(esp_flash_write(_flash, buffer, FLASH_DEST_ADDR + offset, READSIZE));
        gpio_set_level(ADDON_BLUE_LED, toggle);
        toggle = !toggle;
        memset(buffer, 0, READSIZE);
    }
    
    free(buffer);
}

void erase_ota(int ota) {
    // Get the OTA partition
    esp_partition_subtype_t subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
    if (ota == 1) {
        subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
    } else if(ota != 0) {
        ESP_LOGE(TAG, "Invalid OTA partition number: %d", ota);
        return;
    }

    const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (ota_partition == NULL) {
        ESP_LOGE(TAG, "Failed to get OTA partition!");
        return;
    }
    size_t size = ota_partition->size;
    ESP_LOGI(TAG, "Found OTA partition ota_%d! size: %d", ota, size);

    esp_err_t err = esp_partition_erase_range(ota_partition, 0, ota_partition->size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase OTA partition, error=%d", err);
        return;
    }
}

#endif
