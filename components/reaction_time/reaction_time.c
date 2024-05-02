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

void play_level(int level);

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
        char c;
        int cb = read(stdin_fileno, &c, 1);
        // clang-format off
        if (cb == 0) break;
        // clang-format on
        if (cb < 0) {
            usleep(retry_ms * 1000);
            timeout_ms -= retry_ms;
            continue;
        }

        if (read_bytes == 0 && c != '\e') {
            continue;
        }

        sequence[read_bytes++] = c;
    }

    while (timeout_ms > 0) {
        char c;
        int cb = read(stdin_fileno, &c, 1);
        // clang-format off
        if (cb == 0) break;
        if (cb > 0) continue;
        // clang-format on

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

    /* Running the core logic for the level */
    play_level(*level);

    return ESP_OK;
}

static struct {
    int32_t gpio_num;
    uint8_t repeat;
} btn_status;
static SemaphoreHandle_t btn_mutex;

button_t get_random_button(button_t last_button)
{
    button_t new_button;
    do {
        new_button = BUTTONS[esp_random() % BUTTON_COUNT];
    } while (new_button == last_button);
    return new_button;
}

void play_level(int level)
{
    /* Setting up the button pattern for the current level */
    button_t pattern[16];
    switch (level) {
    case 1:
        assert(sizeof(pattern) == sizeof(LVL1PAT));
        memcpy(pattern, LVL1PAT, sizeof(pattern));
        break;
    case 2:
        assert(sizeof(pattern) == sizeof(LVL2PAT));
        memcpy(pattern, LVL2PAT, sizeof(pattern));
        break;
    case 3:
        pattern[0] = BUTTONS[esp_random() % BUTTON_COUNT];
        for (int i = 1; i < 16; i++) {
            pattern[i] = get_random_button(pattern[i - 1]);
        }
        break;
    default:
        ESP_LOGE(LOG_TAG, "Level %d is invalid. Must be between 1 and 3",
                 level);
        return;
    }

    // TODO Draw the badge

    for (int i = 0; i < 16; i++) {
        if (xSemaphoreTake(btn_mutex, portMAX_DELAY) == pdTRUE) {
            btn_status.gpio_num = 0;
            btn_status.repeat = 0;
            if (level > 1) {
                vTaskDelay((2500 + (esp_random() % 1001)) / portTICK_PERIOD_MS);
            }
            assert(xSemaphoreGive(btn_mutex) == pdTRUE);
        } else {
            ESP_LOGE(LOG_TAG, "Failed to acquire a mutual exclusion lock");
            return;
        }

        if (level == 1) {
            // Allows plenty of time to set the button in the first level
            vTaskDelay((2500 + (esp_random() % 1001)) / portTICK_PERIOD_MS);
        }

        const char *buttons[] = {"UP", "DOWN", "LEFT", "RIGHT"};
        ESP_LOGD(LOG_TAG, "Drawing %s button", buttons[pattern[i] - BUTTON_UP]);

        // TODO Draw the badge's button

        // Value picked considering median is 273ms and average is 284ms
        // humanbenchmark.com/tests/reactiontime/statistics
        vTaskDelay(250 / portTICK_PERIOD_MS);

        if (xSemaphoreTake(btn_mutex, portMAX_DELAY) == pdTRUE) {
            if (btn_status.gpio_num != pattern[i]) {
                printf(ERROR_MESSAGES[esp_random() % ERROR_MESSAGE_COUNT]);
                assert(xSemaphoreGive(btn_mutex) == pdTRUE);
                return;
            }
            if (level > 2 && btn_status.repeat > 0) {
                // Allows button mashing in the second level
                printf("Multiple button clicks are not allowed\n");
                assert(xSemaphoreGive(btn_mutex) == pdTRUE);
                return;
            }
            assert(xSemaphoreGive(btn_mutex) == pdTRUE);
        } else {
            ESP_LOGE(LOG_TAG, "Failed to acquire a mutual exclusion lock");
            return;
        }
    }

    // TODO Look into unlocking LED patterns
    switch (level) {
    case 1:
        printf("FLAG-1\n"); // FIXME First flag
        break;
    case 2:
        printf("FLAG-2\n"); // FIXME Second flag
        break;
    case 3:
        printf("FLAG-3\n"); // FIXME Third flag
        break;
    }

    /* Updating maximum level achieved in storage and the help text */
    if (level < 3) {
        esp_err_t err = write_level(level + 1);
        if (err == ESP_OK) {
            update_cmd_help(level + 1);
        } else {
            ESP_LOGE(LOG_TAG, "Failed to update the achieved level in storage");
        }
    }
}

void button_callback(void *button_handle, void *usr_data)
{
    if (xSemaphoreTake(btn_mutex, (TickType_t)0) == pdTRUE) {
        btn_status.gpio_num = *(int32_t *)usr_data;
        btn_status.repeat = iot_button_get_repeat(button_handle);

        const char *buttons[] = {"UP", "DOWN", "LEFT", "RIGHT"};
        ESP_LOGD(LOG_TAG, "Clicked %dx on %s button", 1 + btn_status.repeat,
                 buttons[btn_status.gpio_num - BUTTON_UP]);

        assert(xSemaphoreGive(btn_mutex) == pdTRUE);
    }
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
        .command = CMD_NAME,
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
