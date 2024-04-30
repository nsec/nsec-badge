#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#if !CONFIG_IDF_TARGET_LINUX
#include <stdio_ext.h>
#endif

#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "iot_button.h"
#include "nvs_flash.h"

#include "badge_png.h"
#include "reaction_time.h"

void flush_write(void)
{
#if !CONFIG_IDF_TARGET_LINUX
    if (__fbufsize(stdout) > 0) {
        fflush(stdout);
    }
#endif
    fsync(fileno(stdout));
}

int graphics_probe(void)
{
    /* Switch to non-blocking mode */
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    if (fcntl(stdin_fileno, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }

    /* Query for graphics protocol support */
    uint32_t image_id = esp_random();
    fprintf(stdout, "\e_Gi=%lu,a=q,s=1,v=1;AAAAAA\e\\", image_id);
    fprintf(stdout, "\e[c");
    flush_write();

    /* Attempt to read response */
    const int retry_ms = 10;
    int timeout_ms = 500;
    size_t read_bytes = 0;
    char sequence[4] = {0};

    while (timeout_ms > 0 && read_bytes < sizeof(sequence) - 1) {
        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;

        char c;
        int cb = read(stdin_fileno, &c, 1);
        /* clang-format off */
        if (cb == 0) break;
        if (cb < 0) continue;
        /* clang-format on */

        if (read_bytes == 0 && c != '\e') {
            continue;
        }

        sequence[read_bytes++] = c;
    }

    char c;
    while (timeout_ms > 0 && read(stdin_fileno, &c, 1) != 0) {
        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;
    }

    /* Restore previous mode */
    if (fcntl(stdin_fileno, F_SETFL, flags) == -1) {
        return -1;
    }

    if (strcmp(sequence, "\e_G") != 0)
        return -2;
    return 0;
}

button_t get_random_button(button_t last_button)
{
    button_t new_button;
    do {
        new_button = esp_random() % BUTTON_COUNT;
    } while (new_button == last_button);
    return new_button;
}

esp_err_t write_level(uint8_t level)
{
    nvs_handle_t nvs;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_u8(nvs, NVS_LEVEL_KEY, level);
    if (err != ESP_OK) {
        goto close;
    }

    err = nvs_commit(nvs);

close:
    nvs_close(nvs);
    return err;
}

esp_err_t read_level(uint8_t *level)
{
    nvs_handle_t nvs;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_get_u8(nvs, NVS_LEVEL_KEY, level);

    nvs_close(nvs);
    return err;
}

struct {
    struct arg_int *level;
    struct arg_end *end;
} rt_cmd_args;

char rt_cmd_help[] = "Can you react as fast as a neutrophil?";

void update_cmd_help(uint8_t max_level)
{
    if (max_level == 2) {
        strncpy(rt_cmd_help + 27, "basophil?", 10);
    } else if (max_level == 3) {
        strncpy(rt_cmd_help + 27, "eosinophil?", 12);
    }
}

int rt_cmd(int argc, char **argv)
{
    const char *progname = argv[0];

    /* Probing for graphics protocol support */
    int probe_result = graphics_probe();
    if (probe_result == -1) {
        int errnum = errno;
        ESP_LOGE(progname, "Failed querying for graphics protocol support: %s",
                 strerror(errnum));
        return ESP_OK;
    } else if (probe_result == -2) {
        // ESP_LOGE(progname, "Graphics protocol not supported");
        ESP_LOGI("trivia_time", "What are ASCII Escape Codes?");
        return ESP_OK;
    }

    /* Reading maximum level achieved from storage */
    uint8_t max_level = 1;
    esp_err_t err = read_level(&max_level);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(progname, "Failed reading level from storage: %s",
                 esp_err_to_name(err));
    }

    /* Parsing and validating level selection */
    int *level = rt_cmd_args.level->ival;
    *level = max_level;

    int nerrors = arg_parse(argc, argv, (void **)&rt_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, rt_cmd_args.end, progname);
        return ESP_OK;
    }
    if (*level < 1 || *level > 3) {
        fprintf(stderr, "%s: invalid value \"%d\" to option -l|--level=<1-3>\n",
                progname, *level);
        return ESP_OK;
    }

    if (*level > max_level) {
        *level = max_level;
        ESP_LOGW(progname, "Capped to highest level unlocked: %d", max_level);
    }

    return ESP_OK;
}

void button_callback(void *button_handle, void *usr_data)
{
}

esp_err_t iot_button_register(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        const button_config_t gpio_btn_cfg = {
            .type = BUTTON_TYPE_GPIO,
            .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
            .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
            .gpio_button_config = {
                .gpio_num = BUTTONS[i],
                .active_level = 0,
            }};

        button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
        if (!gpio_btn) {
            return ESP_FAIL;
        }

        esp_err_t err = iot_button_register_cb(
            gpio_btn, BUTTON_PRESS_DOWN, button_callback, (void *)&BUTTONS[i]);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

void register_reaction_time_cmd(void)
{
    rt_cmd_args.level = arg_int0("l", "level", "<1-3>", "challenge level");
    rt_cmd_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "reaction_time",
        .help = rt_cmd_help,
        .hint = NULL,
        .func = &rt_cmd,
        .argtable = &rt_cmd_args,
    };

    /* Updating white cell type in help text */
    uint8_t max_level = 1;
    esp_err_t err = read_level(&max_level);
    if (err != ESP_ERR_NVS_NOT_FOUND)
        ESP_ERROR_CHECK(err);
    update_cmd_help(max_level);

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

    /* Setting up a lock for button press events */
    btn_mutex = xSemaphoreCreateMutex();
    assert(btn_mutex != NULL);

    ESP_ERROR_CHECK(iot_button_register());
}
