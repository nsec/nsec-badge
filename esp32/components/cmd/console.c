/* Console example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "nvs.h"
#include "cmd_system.h"
#include "cmd_nvs.h"
#include "console.h"

static const char* TAG = "console";
#define PROMPT_STR "nsec" 

static void initialize_console(void)
{
    /* drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* minicom, screen, idf_monitor send cr when enter key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    /* configure uart. note that ref_tick is used so that the baud rate remains
     * correct while apb frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
            .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .source_clk = UART_SCLK_REF_TICK,
    };
    /* install uart driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );
    ESP_ERROR_CHECK( uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config) );

    /* tell vfs to use uart driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    /* initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = 256,
#if config_log_colors
            .hint_color = atoi(log_color_cyan)
#endif
    };
    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* configure linenoise line completion library */
    /* enable multiline editing. if not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* set command history size */
    linenoiseHistorySetMaxLen(100);

    /* set command maximum length */
    linenoiseSetMaxLineLen(console_config.max_cmdline_length);

    /* don't return empty lines */
    linenoiseAllowEmpty(false);

    /* load command history from filesystem */
    // linenoisehistoryload(history_path);
}

void console_task(void *args)
{
    initialize_console();

    /* register commands */
    esp_console_register_help_command();
    register_system();
    // register_nvs();

    /* prompt to be printed before each line.
     * this can be customized, made dynamic, etc.
     */
    const char* prompt = LOG_COLOR_I PROMPT_STR "> " LOG_RESET_COLOR;

    printf("\n"
           "this is an example of esp-idf console component.\n"
           "type 'help' to get the list of commands.\n"
           "use up/down arrows to navigate through command history.\n"
           "press tab when typing command name to auto-complete.\n"
           "press enter or ctrl+c will terminate the console environment.\n");

    /* figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        printf("\n"
               "your terminal application does not support escape sequences.\n"
               "line editing and history features are disabled.\n"
               "on windows, try using putty instead.\n");
        linenoiseSetDumbMode(1);
#if config_log_colors
        /* since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = PROMPT_STR "> ";
#endif //config_log_colors
    }

    /* main loop */
    while(true) {
        /* get a line using linenoise.
         * the line is returned when enter is pressed.
         */
        char* line = linenoise(prompt);
        if (line == NULL) { /* break on eof or error */
            break;
        }
        /* add the command to the history if not empty*/
        if (strlen(line) > 0) {
            linenoiseHistoryAdd(line);
        }

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }

    ESP_LOGE(TAG, "Error or end-of-input, terminating console");
    esp_console_deinit();
}
