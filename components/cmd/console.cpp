// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include "esp_log.h"
#include "esp_console.h"

#include "ota_init.h"
#if CONFIG_NSEC_BUILD_CTF
#include "wifi.h"
#endif

static const char* TAG = "console";
#define PROMPT_STR "nsec"

extern "C" void console_init()
{
    ESP_LOGI(TAG, "Starting console initialization");

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

    printf(
        LOG_COLOR(LOG_COLOR_BLUE) "\n\n:::.    :::.    ...    :::::::.. :::::::::::: ::   .:  .::::::..,::::::   .,-:::::   \n" \
        LOG_COLOR(LOG_COLOR_BLUE) "`;;;;,  `;;; .;;;;;;;. ;;;;``;;;;;;;;;;;;'''',;;   ;;,;;;`    `;;;;'''' ,;;;'````'   " LOG_COLOR(LOG_COLOR_RED ";5") "    ,;'``;.   ,;;,   ,;'``;.;'`';;,  \n" \
        LOG_COLOR(LOG_COLOR_BLUE) "  [[[[[. '[[,[[     \\[[,[[[,/[[['     [[    ,[[[,,,[[['[==/[[[[,[[cccc  [[[         " LOG_COLOR(LOG_COLOR_RED ";5") "     ''  ,[[',['  [n  ''  ,[['  .n[[  \n" \
        LOG_COLOR(LOG_COLOR_BLUE) "  $$$ \"Y$c$$$$$,     $$$$$$$$$c       $$    \"$$$\"\"\"$$$  '''    $$$\"\"\"\"  $$$    " LOG_COLOR(LOG_COLOR_RED ";5") "          .c$$P'  $$    $$ .c$$P'   ``\"$$$.\n" \
        LOG_COLOR(LOG_COLOR_BLUE) "  888    Y88\"888,_ _,88P888b \"88bo,   88,    888   \"88o88b    dP888oo,__`88bo,__,o,  " LOG_COLOR(LOG_COLOR_RED ";5") "   d88 _,oo,Y8,  ,8\"d88 _,oo, ,,o888\"\n" \
        LOG_COLOR(LOG_COLOR_BLUE) "  MMM     YM  \"YMMMMMP\" MMMM   \"W\"    MMM    MMM    YMM \"YMmMY\" \"\"\"\"YUMMM \"YUMMMMMP\"  " LOG_COLOR(LOG_COLOR_RED ";5") "  MMMUP*\"^^ \"YmmP  MMMUP*\"^^ YMMP\"  \n\n\n" LOG_RESET_COLOR);

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ESP_LOGI(TAG, "Console started");
}
