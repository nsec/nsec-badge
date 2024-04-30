// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

#include "ota_init.h"
#include "cmd_sys.h"
#if CONFIG_NSEC_BUILD_CTF
#include "wifi.h"
#endif

static const char* TAG = "console";
#define PROMPT_STR LOG_RESET_COLOR "nsec"

extern "C" void console_init()
{
    ESP_LOGD(TAG, "Starting console initialization");

    esp_console_repl_t *repl = nullptr;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 80;

    /* Register commands */
    esp_console_register_help_command();
    register_ota_cmd();
    console_register_cmd_sys();
#if CONFIG_NSEC_BUILD_CTF
    // CTF-only commands
    register_wifi_cmd();
#endif

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
#else
#error Unsupported console type
#endif

    /* Clear the screen before printing the banner. */
    linenoiseClearScreen();

    printf(
        "\n\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "███╗   ██╗ ██████╗ ██████╗ ████████╗██╗  ██╗███████╗███████╗ ██████╗\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "████╗  ██║██╔═══██╗██╔══██╗╚══██╔══╝██║  ██║██╔════╝██╔════╝██╔════╝\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "██╔██╗ ██║██║   ██║██████╔╝   ██║   ███████║███████╗█████╗  ██║     \n"
        LOG_COLOR(LOG_COLOR_BLUE)     "██║╚██╗██║██║   ██║██╔══██╗   ██║   ██╔══██║╚════██║██╔══╝  ██║     \n"
        LOG_COLOR(LOG_COLOR_BLUE)     "██║ ╚████║╚██████╔╝██║  ██║   ██║   ██║  ██║███████║███████╗╚██████╗\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                   ██████╗  ██████╗ ██████╗ ██╗  ██╗\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                   ╚════██╗██╔═████╗╚════██╗██║  ██║\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                    █████╔╝██║██╔██║ █████╔╝███████║\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                   ██╔═══╝ ████╔╝██║██╔═══╝ ╚════██║\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                   ███████╗╚██████╔╝███████╗     ██║\n"
        LOG_COLOR(LOG_COLOR_RED ";5") "                   ╚══════╝ ╚═════╝ ╚══════╝     ╚═╝\n"
	LOG_RESET_COLOR
	"\n\n\n");

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ESP_LOGD(TAG, "Console started");
}
