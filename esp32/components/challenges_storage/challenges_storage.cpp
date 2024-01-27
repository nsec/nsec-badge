/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_flash.h"
#include "driver/spi_master.h"

#include "save.h"

#include "challenges_storage.h"
#include "ota_operations.h"
#include "spi_operations.h"
#include "flash_operations.h"

#define DATA_PIN_1 33
#define LED_TYPE WS2811

static esp_flash_t* flash = NULL;
static int64_t start_time = 0;

void challenges_storage_init() {
    if (Save::save_data.raw_spi_mode) {
        // Initialize the external SPI Device
        init_ext_spi();
        printf("Booted in RAW SPI mode\n");
    } else {
        // Initialize the external SPI Flash chip
        flash = init_ext_flash();
        printf("Booted in regular flash mode\n");
    }
}

void challenges_storage_start() {
    start_time = esp_timer_get_time();
}

void challenges_storage_end() {
    int64_t end_time = esp_timer_get_time();
    printf("Execution time: %.6f seconds\n", (double)(end_time - start_time) / 1000000.0);
    start_time = 0;
}

static const char *TAG = "challenges_storage";

unsigned int parse_address(int argc, char **argv) {
    if (argc < 2) return 0;
    unsigned int address = 0;
    if(sscanf(argv[1], "%x", &address)) {
        ESP_LOGD(TAG, "Parsed address: 0x%02x", address);
    } else {
        printf("Invalid hexadecimal address format! Going with 0x00.\n");
        address = 0x00;
    }
    return address;
}

static int challenge_storage(int argc, char **argv) {
    uint16_t select_challenge = 1;

     if (flash == NULL) {
        ESP_LOGE(TAG, "Can't execute challenge_storage, flash is NULL.");
        return 0;
    }
    challenges_storage_start();

    if (argc >= 2) {
        select_challenge = atoi(argv[1]);
    }
    if (select_challenge == 4) {
        flash_read(flash, 128);
    } else if (select_challenge == 5) {
        flash_write_flag(flash, 0x00);
    } else if (select_challenge == 11) {
        storage_read_from_ota(1, flash);
    } else if (select_challenge == 12) {
        storage_write_to_ota(0, flash);
    } else if (select_challenge == 13) {
        ESP_ERROR_CHECK(esp_flash_erase_region(flash, 0x00000, 4096));
    } else if (select_challenge == 17) {
        erase_ota(0);
    } else if (select_challenge == 19) {
        ESP_ERROR_CHECK(esp_flash_erase_chip(flash));
    }

    challenges_storage_end();

    return 0;
}

static int raw_toggle(int argc, char **argv) {
    Save::save_data.raw_spi_mode = !Save::save_data.raw_spi_mode;
    Save::write_save();
    printf("save_data.raw_spi_mode = %s\n", Save::save_data.raw_spi_mode ? "true" : "false");
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_restart();
    return 0;
}

static int raw_read_register1(int argc, char **argv) {
    full_duplex_spi_readR1();
    return 0;
}

static int raw_write_register1(int argc, char **argv) {
    if (argc <= 1) {
        printf("Usage: raw_write_register [value]\n");
        return 0;
    }
    full_duplex_spi_writeSR1(static_cast<uint8_t>(parse_address(argc, argv)));
    return 0;
}

// TODO maybe rename to read_flag
static int read_first_128(int argc, char **argv) {
    uint8_t* buffer = (uint8_t*)malloc(1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    ESP_ERROR_CHECK(esp_flash_read(flash, buffer, 0x010, 1));
    if (buffer[0] == 0xAA) {
        flash_read(flash, 128);
    } else {
        printf("0x010 value is not 0xAA! exiting...\n");
    }
    free(buffer);
    return 0;
}

// TODO maybe rename to read_flag2
static int read_later_128(int argc, char **argv) {
    uint8_t* buffer = (uint8_t*)malloc(1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    uint32_t addr2 = 0x010010;
    uint32_t sect2 = 0x010000;
    uint32_t addr1 = 0x020010;
    uint32_t sect1 = 0x020000;
    if (flash->size == 1048576) {
        // W25Q80DV 8MBITS solve: 0x28
    } else if (flash->size == 8388608) {
        // W25Q64FV 64MBITS solve: 0x24
        uint32_t addr2 = 0x010010;
        uint32_t sect2 = 0x010000;
        uint32_t addr1 = 0x020010;
        uint32_t sect1 = 0x020000;
    } else {
        printf("Unknown flash size! exiting...\n");
        return 1;
    }
    
    uint8_t w[] = {0xBB};
    printf(LOG_COLOR(LOG_COLOR_BLUE) "Writing 0x%02X to 0x%06lX...\n", w[0], addr1);
    ESP_ERROR_CHECK(esp_flash_erase_region(flash, sect1, 4096));
    ESP_ERROR_CHECK(esp_flash_write(flash, w, addr1, 1));

    ESP_ERROR_CHECK(esp_flash_read(flash, buffer, addr1, 1));
    if (buffer[0] != w[0]) {
        printf("Looks like I can't write at 0x%06lX, but that's what I want!\n", addr1);
        // Write at addr1 doesn't work, OK!
        w[0] = {0xCC};
        printf("Writing 0x%02X to 0x%06lX...\n", w[0], addr2);
        ESP_ERROR_CHECK(esp_flash_erase_region(flash, sect2, 4096));
        ESP_ERROR_CHECK(esp_flash_write(flash, w, addr2, 1));
        ESP_ERROR_CHECK(esp_flash_read(flash, buffer, addr2, 1));
        if (buffer[0] == w[0]) {
            // Write at addr2 works, OK!
            printf("Looks like I can write at 0x%06lX, and that's what I want!\n" LOG_RESET_COLOR, addr2);
            flash_read_at(flash, 128, 0x011000);
        } else {
            printf("Can't write 0x%02X value at 0x%06lX! exiting...\n", w[0], addr2);
        }
        
    } else {
        w[0] = {0xAA};
        printf("0x%06lX value is 0x%02X! erasing it...\n", addr1, buffer[0]);
        ESP_ERROR_CHECK(esp_flash_erase_region(flash, sect1, 4096));
        //ESP_ERROR_CHECK(esp_flash_write(flash, w, addr1, 1));
    }
    printf(LOG_RESET_COLOR);
    free(buffer);
    return 0;
}


static int write_to_0x10(int argc, char **argv) {
    uint8_t w[] = {0xAA};
    printf("Writing 0xAA to 0x010...\n");
    ESP_ERROR_CHECK(esp_flash_write(flash, w, 0x010, 1));
    uint8_t* buffer = (uint8_t*)malloc(128);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    ESP_ERROR_CHECK(esp_flash_read(flash, buffer, 0x010, 1));
    printf("0x010 value is: %02X\n", buffer[0]);

    free(buffer);
    return 0;
}

void register_challenges_storage(void) {
    const esp_console_cmd_t cmd = {
        .command = "storage",
        .help = "Run the storage challenge stuff\n",
        .hint = "[4,11,12,13,17,19]",
        .func = &challenge_storage,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );

    const esp_console_cmd_t cmd2 = {
        .command = "raw_toggle",
        .help = "Toggle RAW access to SPI Flash\n",
        .hint = "",
        .func = &raw_toggle,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd2) );

    const esp_console_cmd_t cmd3 = {
        .command = "raw_read_register1",
        .help = "Read registers from SPI Flash\n",
        .hint = "",
        .func = &raw_read_register1,
        .argtable = NULL,        
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd3) );
    const esp_console_cmd_t cmd4 = {
        .command = "raw_write_register1",
        .help = "Write register to SPI Flash\n",
        .hint = "[value]",
        .func = &raw_write_register1,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd4) );

    const esp_console_cmd_t cmd5 = {
        .command = "read_first_128",
        .help = "Read first 128 bytes on the chip, only if value of addr 0x10 is 0xAA\n",
        .hint = "",
        .func = &read_first_128,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd5) );

    const esp_console_cmd_t cmd6 = {
        .command = "write_to_0x10",
        .help = "Write 0xAA to addr 0x10\n",
        .hint = "",
        .func = &write_to_0x10,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd6) );

    const esp_console_cmd_t cmd7 = {
        .command = "read_later_128",
        .help = "Read 128 bytes further in the chip, only if value of addr 0x10 is not 0xBB and value of addr 0x200010 is 0xAA\n",
        .hint = "",
        .func = &read_later_128,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd7) );

}

