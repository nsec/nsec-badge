#include <string.h>
#include "esp_random.h"
#include "esp_flash.h"
#include "ota_operations.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"


#define READSIZE (4096 * 1)
#define FLASH_DEST_ADDR 0x400000

static const char *TAG = "ota_operations";

void challenge_storage1_code(int _delay) {
    ESP_LOGI(TAG, "in _code!\n");
    int DELAY = 200;
    if (_delay) DELAY = _delay;
}

void storage_write_to_ota(int ota, esp_flash_t* _flash) {
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
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Failed to find OTA 0 partition");
        return;
    }

    err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, error=%d", err);
        return;
    }
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