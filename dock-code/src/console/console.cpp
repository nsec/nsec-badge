#include <stdio.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

//#include "qkd/qkd.h"
#include "safe/safe.h"

static const char *TAG = "console";

static void clear_and_print_banner()
{
    /* Clear the screen before printing the banner. */
    linenoiseClearScreen();

    printf("====================================\n");
    printf("Padraignix - QKD Safe Dock Platform\n");
    printf("====================================\n");
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
    //register_bb84_cmd();
    //register_quantum_cmd();
    //register_calibrate_cmd();
    //register_qkd_cmd();
    register_safe_cmd();
}

extern "C" void console_init()
{
    ESP_LOGD(TAG, "Starting console initialization");

    esp_console_repl_t *repl = nullptr;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = "> ";
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

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    ESP_LOGD(TAG, "Console started");
}