#include <string.h>
#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "driver/spi_common.h"
#include "soc/spi_pins.h"
#include "esp_log.h"
#include "storage_settings.h"
#include "save.h"
#include "crypto_operations.h"

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

void print_cryptd_flag(esp_flash_t* _flash, uint32_t flag_addr) {
    uint8_t cipher[16];
    char* read_buffer2 = (char*)malloc(sizeof(cipher));
    ESP_ERROR_CHECK(esp_flash_read(_flash, read_buffer2, flag_addr, sizeof(cipher)));
    memcpy(cipher, read_buffer2, sizeof(cipher));
    free(read_buffer2);
    uint8_t decrypted_flag2[16] = {0x00};
    decrypt_flag(cipher, decrypted_flag2);
    print_16str(decrypted_flag2);
}

