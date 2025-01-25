// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <esp_console.h>
#include <esp_log.h>
#include <esp_system.h>

//#ifdef CONFIG_NSEC_BUILD_CONFERENCE
//#include <globals.hpp>
//#endif

#include "cmd_sys.h"

static const char *TAG = "console";

static int sys_cmd_reboot(int argc __attribute__((unused)),
                          char **argv __attribute__((unused)))
{
    printf("Restarting...\n");

    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(200));

//#ifdef CONFIG_NSEC_BUILD_CONFERENCE
//    nsec::g::the_badge->clear_leds();
//#endif
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

}
