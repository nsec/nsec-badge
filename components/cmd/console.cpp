// SPDX-FileCopyrightText: 2025 NorthSec
// SPDX-FileCopyrightText: 2025 NorthSec
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

#include "cmd_sys.h"
#include "badge_ssd1306_helper.hpp"
#include "badge_nsec_logo.h"

#if CONFIG_NSEC_BUILD_CTF
#include "quantum.h"
#include "quantum_gates.h"
#include "calibration.h"
#include "qkd.h"
#include "codenames.h"
#include "safe_unlock.h"
#endif

#include <SmartLeds.h>

static const char *TAG = "console";
#define PROMPT_STR LOG_RESET_COLOR "nsec"

static void clear_and_print_banner()
{
    /* Clear the screen before printing the banner. */
    linenoiseClearScreen();

    badge_lcd_nsec_logo();

    printf(
        "\n\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "                                       __/___                                          .\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "                                 _____/______|                                         .\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "                         _______/_____\\_______\\_____                                   . .\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "                         \\              < < <       |                                 ...\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           \\~~~~~/\n"
        LOG_COLOR(LOG_COLOR_BLUE)     "  _   _  ____  _____ _______ _    _  _____ ______ _____   ___   ___ ___  _____       \\   /\n"
        LOG_COLOR(LOG_COLOR_BLUE)     " | \\ | |/ __ \\|  __ \\__   __| |  | |/ ____|  ____/ ____| |__ \\ / _ \\__ \\| ____|       \\ /\n"
        LOG_COLOR(LOG_COLOR_BLUE)     " |  \\| | |  | | |__) | | |  | |__| | (___ | |__ | |         ) | | | | ) | |__          V\n"
        LOG_COLOR(LOG_COLOR_BLUE)     " | . ` | |  | |  _  /  | |  |  __  |\\___ \\|  __|| |        / /| | | |/ /|___ \\         |\n"
        LOG_COLOR(LOG_COLOR_BLUE)     " | |\\  | |__| | | \\ \\  | |  | |  | |____) | |___| |____   / /_| |_| / /_ ___) |        |\n"
        LOG_COLOR(LOG_COLOR_BLUE)     " |_| \\_|\\____/|_|  \\_\\ |_|  |_|  |_|_____/|______\\_____| |____|\\___/____|____/        ---\n"
#if !CONFIG_NSEC_BUILD_CONFERENCE
        LOG_COLOR(LOG_COLOR_CYAN) "\n\nYou are on the "
#endif
#if CONFIG_NSEC_BUILD_CTF
                                  "CTF firmware\n"
#endif
        LOG_RESET_COLOR "\n\n\n");
}

static int cmd_clear(int argc __attribute__((unused)),
                     char **argv __attribute__((unused)))
{
    clear_and_print_banner();

    return ESP_OK;
}

static void console_register_cmd_clear()
{
    const esp_console_cmd_t cmd = {
        .command = "clear",
        .help = "Clear the console",
        .hint = nullptr,
        .func = &cmd_clear,
        .argtable = nullptr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void register_commands()
{
    esp_console_register_help_command();
    console_register_cmd_clear();
    console_register_cmd_sys();
#if CONFIG_NSEC_BUILD_CTF
    register_quantum_cmd();
    register_calibrate_cmd();
    register_qkdinit_cmd();
    register_qkd_cmd();
    register_qkdnoisy_cmd();
    register_codenames_cmd();
    register_qkd_safe_cmd();
#endif
}

extern "C" void console_init()
{
    ESP_LOGD(TAG, "Starting console initialization");

    esp_console_repl_t *repl = nullptr;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 200;

    register_commands();

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

    clear_and_print_banner();
    
    //Weird hack to account for firmware swap. making sure leds are initialized
    #if CONFIG_NSEC_BUILD_CTF
        esp_log_level_set("gpio", ESP_LOG_WARN);
        SmartLed leds(LED_WS2812B, 18, 8, 0);
        for (int i = 0; i < 18; i++) {
            leds[i] = Rgb{0, 0, 0};
        }
        leds.show();
        leds.wait();      
    #endif  
    
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ESP_LOGD(TAG, "Console started");
}
