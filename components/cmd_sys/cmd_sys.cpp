// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <esp_console.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <string.h>

#ifdef CONFIG_NSEC_BUILD_CONFERENCE
#include <badge/globals.hpp>
#include <utils/board.hpp>
#include <SmartLeds.h>
#endif

#include "cmd_sys.h"

static const char *TAG = "console";

#ifdef CONFIG_NSEC_BUILD_CONFERENCE
// Helper function to minimize LED brightness using direct GPIO control
static void clear_leds()
{
    printf("Setting LED GPIO to input (low power mode)...\n");
    
    // Configure LED control pin as input with pull-down to minimize brightness
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;  // Input mode to stop driving the signal
    io_conf.pin_bit_mask = (1ULL << nsec::board::neopixel::ctrl_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;  // Pull down to ensure low state
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    printf("LED pin configured as input with pull-down.\n");
}
#endif

static int sys_cmd_reboot(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)))
{
    printf("Restarting...\n");

    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(200));

#ifdef CONFIG_NSEC_BUILD_CONFERENCE
    clear_leds();
#endif
    esp_restart();

    // That's not gonna happen...
    return ESP_OK;
}

static int sys_cmd_factory_reset(int argc __attribute__((unused)),
                                 char **argv __attribute__((unused)))
{
    printf("Factory reset...\n");

    if (nvs_flash_erase() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear the NVS partition");
    }

    // Reboot
    sys_cmd_reboot(0, nullptr);

    // That's not gonna happen...
    return ESP_OK;
}

static int sys_cmd_firmware_select(int argc, char **argv)
{
    const esp_partition_t *factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    const esp_partition_t *ota_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    const esp_partition_t *boot_partition = esp_ota_get_boot_partition();

    // Determine which firmware is currently running
    bool is_conf_running = (running_partition->subtype == ESP_PARTITION_SUBTYPE_APP_FACTORY);
    bool is_ctf_running = (running_partition->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_0);

    if (argc != 2) {
        printf("Current firmware: %s\n", is_conf_running ? "conf" : (is_ctf_running ? "ctf" : running_partition->label));
        printf("Boot partition: %s\n", boot_partition == factory_partition ? "conf" : 
                                   (boot_partition == ota_partition ? "ctf" : boot_partition->label));
        printf("Usage: firmware_select [conf|ctf]\n");
        return ESP_OK;
    }

    if (strcmp(argv[1], "conf") == 0) {
        if (factory_partition == NULL) {
            printf("Conference firmware not found!\n");
            return ESP_ERR_NOT_FOUND;
        }
        
        // If already running conference firmware, no need to change
        if (running_partition == factory_partition) {
            printf("Already running conference firmware.\n");
            return ESP_OK;
        }
        
        printf("Switching to conference firmware...\n");
        esp_err_t err = esp_ota_set_boot_partition(factory_partition);
        if (err != ESP_OK) {
            printf("Failed to set boot partition: %s\n", esp_err_to_name(err));
            return err;
        }
        
        printf("Successfully set boot partition to conference firmware\n");
        printf("Rebooting in 2 seconds...\n");
        
        // Delay to allow serial output to finish
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // Reboot to apply changes
        esp_restart();
    } else if (strcmp(argv[1], "ctf") == 0) {
        if (ota_partition == NULL) {
            printf("CTF firmware not found!\n");
            return ESP_ERR_NOT_FOUND;
        }
        
        // If already running CTF firmware, no need to change
        if (running_partition == ota_partition) {
            printf("Already running CTF firmware.\n");
            return ESP_OK;
        }
        
        printf("Switching to CTF firmware...\n");
        esp_err_t err = esp_ota_set_boot_partition(ota_partition);
        if (err != ESP_OK) {
            printf("Failed to set boot partition: %s\n", esp_err_to_name(err));
            return err;
        }
        
        printf("Successfully set boot partition to CTF firmware\n");

#ifdef CONFIG_NSEC_BUILD_CONFERENCE
        // If switching from conference to CTF, clear LEDs to prevent freezing
        printf("Cleaning up LED state before reboot...\n");
        clear_leds();
#endif
        printf("Rebooting...\n");
        // Delay to allow serial output to finish and LED state to update
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // Reboot to apply changes
        esp_restart();
    } else {
        printf("Invalid firmware selection. Use 'conf' or 'ctf'\n");
    }

    return ESP_OK;
}

extern "C" void console_register_cmd_sys(void)
{
    const esp_console_cmd_t cmd_reboot = {
        .command = "reboot",
        .help = "Reboot the system",
        .hint = nullptr,
        .func = &sys_cmd_reboot,
        .argtable = nullptr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_reboot));

    const esp_console_cmd_t cmd_firmware_select = {
        .command = "firmware_select",
        .help = "Select which firmware to boot (conf or ctf)",
        .hint = "[conf|ctf]",
        .func = &sys_cmd_firmware_select,
        .argtable = nullptr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_firmware_select));
}
