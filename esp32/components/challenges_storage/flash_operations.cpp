#include <string.h>
#include "esp_flash.h"
#include "esp_log.h"

static const char *TAG = "flash_operations";

void flash_read(esp_flash_t* _flash, uint16_t size) {
    if (size < 1) size = 128;
    uint8_t* buffer = (uint8_t*)malloc(size);
    if (buffer == NULL) {
        ESP_LOGI(TAG, "Failed to allocate memory for buffer!");
        return;
    }
    ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, 0x0, size));
    for (int i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    free(buffer);
}

void flash_read_at(esp_flash_t* _flash, uint16_t custom_param, unsigned int address) {
    if (custom_param < 1) custom_param = 128;

    const int READSIZE = custom_param;
    uint8_t* buffer = (uint8_t*)malloc(READSIZE);
    if (buffer == NULL) {
        ESP_LOGI(TAG, "Failed to allocate memory for buffer!");
        return;
    }
    ESP_ERROR_CHECK(esp_flash_read(_flash, buffer, address, READSIZE));
    for (int i = 0; i < READSIZE; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    free(buffer);
}

void flash_write_flag(esp_flash_t* _flash, unsigned int address) {
    const char* buffer = "FLAG-CTFFLASH24";
    ESP_ERROR_CHECK(esp_flash_write(_flash, buffer, address, strlen(buffer)));
}

