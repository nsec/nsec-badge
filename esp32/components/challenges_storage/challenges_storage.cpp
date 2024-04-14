// Code for the CTF Addon challenges 2024
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

#include "ota_init.h"
#include "ota_actions.h"

#include "challenges_storage.h"
#include "spi_operations.h"
#include "flash_operations.h"
#include "crypto_operations.h"

// For flag2 challenge
#define SECT_WRITE_PROTECTED 0x050000
#define SECT_WRITABLE 0x080000
#define FLAG_2_ADDR 0x020000

// For when the add on is plugged in
#define FLAG_PLUG_ME_ADDR 0x022000

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

#if CTF_ADDON_ADMIN_MODE
static const char *TAG = "challenges_storage";
#else
static const char *TAG = "ctf_addon";
#endif

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

#if CTF_ADDON_ADMIN_MODE

#include "esp_partition.h"
#include "esp_ota_ops.h"

void challenges_storage_start() {
    start_time = esp_timer_get_time();
}

void challenges_storage_end() {
    int64_t end_time = esp_timer_get_time();
    printf("Execution time: %.6f seconds\n", (double)(end_time - start_time) / 1000000.0);
    start_time = 0;
}

unsigned int parse_address_admin(int argc, char **argv) {
    if (argc < 3) return 0;
    unsigned int address = 0;
    if(sscanf(argv[2], "%x", &address)) {
        ESP_LOGI(TAG, "Parsed address: 0x%02x", address);
    } else {
        ESP_LOGI(TAG, "Invalid hexadecimal address format! Going with 0x00.");
        address = 0x00;
    }
    return address;
}

unsigned int write_encrypted_flag(char * flag_name, char * flag_value, esp_flash_t* flash, uint32_t addr) {
    printf("encrypting %s...\n", flag_name);
    uint8_t plaintext_flag2[16];
    memcpy(plaintext_flag2, flag_value, 16);

    uint8_t encrypted_flag2[16] = {0x00};
    encrypt_flag(plaintext_flag2, encrypted_flag2);
    printf("encrypted %s: ", flag_name);
    print_bin2hex(encrypted_flag2, 16);

    printf("writing encrypted %s...\n", flag_name);
    esp_flash_erase_region(flash, addr, 4096);
    char* encrypted_flag2_str = (char*)malloc(sizeof(encrypted_flag2) + 1); // +1 for the null terminator
    if (encrypted_flag2_str == NULL) {
        printf("Failed to allocate memory for encrypted %s str.\n", flag_name);
        return 1;
    }
    memcpy(encrypted_flag2_str, encrypted_flag2, sizeof(encrypted_flag2));
    encrypted_flag2_str[sizeof(encrypted_flag2)] = '\0';
    ESP_ERROR_CHECK(esp_flash_write(flash, encrypted_flag2_str, addr, strlen(encrypted_flag2_str)));
    free(encrypted_flag2_str);

    uint8_t cipher[16];
    char* read_buffer2 = (char*)malloc(sizeof(cipher));
    ESP_ERROR_CHECK(esp_flash_read(flash, read_buffer2, addr, sizeof(cipher)));
    memcpy(cipher, read_buffer2, sizeof(cipher));
    free(read_buffer2);

    uint8_t decrypted_flag2[16] = {0x00};
    decrypt_flag(encrypted_flag2, decrypted_flag2);
    printf("decrypted %s: ", flag_name);
    print_bin2hex(decrypted_flag2, 16);

    if (memcmp(plaintext_flag2, decrypted_flag2, sizeof(decrypted_flag2)) == 0) {
        printf("%s is OK!\n", flag_name);
    } else {
        printf("%s isn't written or encrypted properly.. bailing out!\n", flag_name);
        return 1;
    }
    return 0;
}

static int challenge_storage(int argc, char **argv) {
    uint16_t select_challenge = 1;

    ESP_LOGI(TAG, "ADMIN MODE COMMAND LOADED! IF THIS IS IN PRODUCTION ITS A BIG PROBLEM!");

    // since this is now hybrid mode with raw mode we can't exit when flash is NULL
    //  if (flash == NULL) {
    //     ESP_LOGE(TAG, "Can't execute challenge_storage, flash is NULL.");
    //     return 0;
    // }
    challenges_storage_start();

    if (argc >= 2) {
        select_challenge = atoi(argv[1]);
    }
    if (select_challenge == 4) {
        flash_read(flash, 128);
    } else if (select_challenge == 11) {
        storage_read_from_ota(1, flash);
    } else if (select_challenge == 12) {
        write_flash_to_ota(flash);
    } else if (select_challenge == 13) {
        ESP_ERROR_CHECK(esp_flash_erase_region(flash, 0x00000, 4096));
    } else if (select_challenge == 17) {
        erase_ota(0);
    } else if (select_challenge == 19) {
        ESP_ERROR_CHECK(esp_flash_erase_chip(flash));
    } else if (select_challenge == 222) {
        full_duplex_spi_writeSRN(0x9c, 1); // SRP=1, PROTECTED PORTION=ALL

        // write to SR2
        //full_duplex_spi_writeSRN(static_cast<uint8_t>(parse_address_admin(argc, argv)), 2);
    } else if (select_challenge == 111) {
        printf("auto provisioning of the flash chip and tests...\n");

        // fresh chips should be already erased
        // TODO comment to save time with provision the 150 addons!
        //esp_flash_erase_chip(flash); 

        const char* buffer_flag1 = "FLAG-63QWJK4DCE3";
        printf("writing flag1...\n");
        esp_flash_erase_region(flash, 0x000000, 4096);
        ESP_ERROR_CHECK(esp_flash_write(flash, buffer_flag1, 0x000000, strlen(buffer_flag1)));

        char* read_buffer = (char*)malloc(strlen(buffer_flag1));
        if (read_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for read_buffer!");
            return 1;
        }
        ESP_ERROR_CHECK(esp_flash_read(flash, read_buffer, 0x000000, strlen(buffer_flag1)));
        if (strncmp(read_buffer, buffer_flag1, strlen(buffer_flag1)) == 0) {
            printf("flag1 is OK!\n");
        } else {
            printf("flag1 isn't written properly.. bailing out!\n");
            return 1;
        }
        free(read_buffer);

        const char* buffer_flag0 = "FLAG-NSECCTF24DUMPFLASH";
        printf("writing flag0...\n");
        esp_flash_erase_region(flash, 0x042000, 4096);
        ESP_ERROR_CHECK(esp_flash_write(flash, buffer_flag0, 0x042000, strlen(buffer_flag0)));

        if (write_encrypted_flag("FLAG2", "FLAG-C9K3NI1UOVP", flash, FLAG_2_ADDR) == 1) {
            return 1;
        }

        printf("erasing FLAG2 regions...\n");
        esp_flash_erase_region(flash, SECT_WRITE_PROTECTED, 4096);
        esp_flash_erase_region(flash, SECT_WRITABLE, 4096);

        if (write_encrypted_flag("FLAG_PLUG_ME", "FLAG-PWT4RB47KVA", flash, FLAG_PLUG_ME_ADDR) == 1) {
            return 1;
        }

        // The firmware needs to be available in NSEC_OTA_PARTITION (ota_1)
        esp_partition_subtype_t subtype = NSEC_OTA_PARTITION;
        const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, subtype, NULL);
        esp_app_desc_t desc;
        esp_err_t err = esp_ota_get_partition_description(ota_partition, &desc);
        if (strcmp(desc.project_name, "nsec-ctf-addon") == 0) {
            printf("loading ota_1 data into flash...\n");
            storage_read_from_ota(1, flash);
        } else {
            printf("ERROR: The nsec-ctf-addon firmware needs to be available in ota_1!\n");
        }

        printf("Please swap to RAW mode with 'raw_toggle' and then run 'storage 222'.\n");
    }

    challenges_storage_end();

    return 0;
}
#endif

static int raw_toggle(int argc, char **argv) {
    Save::save_data.raw_spi_mode = !Save::save_data.raw_spi_mode;
    Save::write_save();
    printf("save_data.raw_spi_mode = %s\n", Save::save_data.raw_spi_mode ? "true" : "false");
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_restart();
    return 0;
}

static int raw_read_register123(int argc, char **argv) {
    full_duplex_spi_readR1R2R3();
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
    full_duplex_spi_writeSRN(static_cast<uint8_t>(parse_address(argc, argv)), 1);
    return 0;
}

// TODO maybe rename to read_flag
static int read_first_128(int argc, char **argv) {
    uint8_t* buffer = (uint8_t*)malloc(1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    ESP_ERROR_CHECK(esp_flash_read(flash, buffer, 0x000048, 1));
    if (buffer[0] == 0xAA) {
        flash_read(flash, 128);
    } else {
        printf("0x000048 value is not 0xAA! exiting...\n");
    }
    free(buffer);
    return 0;
}

// TODO maybe rename to read_flag2, add encryption for this flag
static int read_another_128(int argc, char **argv) {
    uint8_t* buffer = (uint8_t*)malloc(1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    // sect1 is the one that should be write protected
    uint32_t addr1 = SECT_WRITE_PROTECTED + 0x48;
    uint32_t sect1 = SECT_WRITE_PROTECTED;
    
    // sect2 is the one that should be writable
    uint32_t addr2 = SECT_WRITABLE + 0x48;
    uint32_t sect2 = SECT_WRITABLE;

    /*if (flash->size == 1048576) {
        // W25Q80DV 8MBITS solve: 0x28
    } else
    */
   // W25Q64JV 64MBITS
    if (flash->size != 8388608) {
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
            flash_read_at(flash, 128, FLAG_2_ADDR);
            printf("Looks like content there is symmetrically encrypted.\n");
        } else {
            printf("Can't write 0x%02X value at 0x%06lX! Exiting.\n", w[0], addr2);
        }
        
    } else {
        w[0] = {0xAA};
        printf("0x%06lX value is 0x%02X! Erasing it and exiting.\n", addr1, buffer[0]);
        ESP_ERROR_CHECK(esp_flash_erase_region(flash, sect1, 4096));
        //ESP_ERROR_CHECK(esp_flash_write(flash, w, addr1, 1));
    }
    printf(LOG_RESET_COLOR);
    free(buffer);
    return 0;
}


static int write_to_0x48(int argc, char **argv) {
    uint8_t w[] = {0xAA};
    printf("Writing 0xAA to 0x000048...\n");
    ESP_ERROR_CHECK(esp_flash_write(flash, w, 0x000048, 1));
    uint8_t* buffer = (uint8_t*)malloc(128);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for buffer!");
        return 1;
    }
    ESP_ERROR_CHECK(esp_flash_read(flash, buffer, 0x000048, 1));
    printf("0x000048 value is: %02X\n", buffer[0]);

    free(buffer);
    return 0;
}

void register_challenges_storage(void) {
    
    #if CTF_ADDON_ADMIN_MODE
    const esp_console_cmd_t cmd = {
        .command = "storage",
        .help = "Run the storage challenge stuff\n",
        .hint = "[4,11,12,13,17,19]",
        .func = &challenge_storage,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    #endif
    
    esp_console_cmd_t cmd2 = {
        .command = "raw_toggle",
        .hint = "",
        .func = &raw_toggle,
        .argtable = NULL,        
    };
    if (Save::save_data.raw_spi_mode) {
        cmd2.help = "Toggle regular access to SPI FLASH. This will reboot the device in the regular flash mode.\n";
    } else {
        cmd2.help = "Toggle RAW access to SPI FLASH. This will reboot the device in RAW SPI mode.\n";
    }
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd2) );

    if (Save::save_data.raw_spi_mode) {
        const esp_console_cmd_t cmd3 = {
            .command = "raw_read_registers",
            .help = "Read registers from SPI FLASH\n",
            .hint = "",
            .func = &raw_read_register123,
            .argtable = NULL,        
        };

        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd3) );
        const esp_console_cmd_t cmd4 = {
            .command = "raw_write_register1",
            .help = "Write register to SPI FLASH.\n",
            .hint = "[hex value]",
            .func = &raw_write_register1,
            .argtable = NULL,        
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd4) );
    } else {
        const esp_console_cmd_t cmd5 = {
            .command = "read_first_128",
            .help = "Read first 128 bytes on SPI FLASH, only if value of addr 0x000048 is 0xAA.\n",
            .hint = "",
            .func = &read_first_128,
            .argtable = NULL,        
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd5) );

        const esp_console_cmd_t cmd6 = {
            .command = "write_to_0x48",
            .help = "Write 0xAA to addr 0x000048 on SPI FLASH.\n",
            .hint = "",
            .func = &write_to_0x48,
            .argtable = NULL,        
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd6) );

        const esp_console_cmd_t cmd7 = {
            .command = "read_another_128",
            .help = "Read 128 bytes somewhere further in SPI FLASH, only under some conditions.\n",
            .hint = "",
            .func = &read_another_128,
            .argtable = NULL,        
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd7) );
    }

}

