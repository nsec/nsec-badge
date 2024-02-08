#include <string.h>
#include "esp_random.h"
#include "esp_flash.h"
#include "ota_init.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

#include "ota_write.h"

#define READSIZE (4096 * 1)
#define FLASH_DEST_ADDR 0x00000

static const char *TAG = "ota_write";
void write_flash_to_ota(esp_flash_t* _flash, esp_partition_t *ota_partition) {
    // Get the OTA partition
    esp_partition_subtype_t subtype = NSEC_OTA_PARTITION;

    const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (ota_partition == NULL) {
        ESP_LOGE(TAG, "Failed to get OTA partition!");
        return;
    }
    size_t size = ota_partition->size;
    ESP_LOGI(TAG, "Found OTA partition ota_%d! size: %d", NSEC_OTA_ID, size);

    esp_ota_handle_t ota_handle;

    // Begin an OTA operation
    esp_err_t err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        return;
    }

    // Buffer for data
    uint8_t* buffer = (uint8_t*)malloc(READSIZE);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return;
    }

    // TODO blink SAO3_IO1 LED while writing to OTA partition
    for (size_t offset = 0; offset < size; offset += READSIZE) {
        // Copy data to buffer
        ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, FLASH_DEST_ADDR + offset, READSIZE));

        // Write data to OTA partition
        err = esp_ota_write(ota_handle, buffer, READSIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed, error=%d", err);
            break;
        }

        memset(buffer, 0, READSIZE);
    }

    // End the OTA operation
    err = esp_ota_end(ota_handle);
    free(buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed, error=%d", err);
        return;
    }
       

    // Set the OTA boot partition
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Failed to find OTA %d partition", NSEC_OTA_ID);
        return;
    }

    ESP_LOGI(TAG, "[SKIPPED] Setting OTA %d as boot partition...", NSEC_OTA_ID);
    /*err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, error=%d", err);
        return;
    }*/
}


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
