#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#if !CONFIG_IDF_TARGET_LINUX
#include <stdio_ext.h>
#endif

#include "esp_console.h"
#include "esp_log.h"
#include "esp_random.h"

#include "badge_png.h"

static const char *LOG_TAG = "reaction_time";

static void flush_write(void)
{
#if !CONFIG_IDF_TARGET_LINUX
    if (__fbufsize(stdout) > 0) {
        fflush(stdout);
    }
#endif
    fsync(fileno(stdout));
}

int probe_graphics(void)
{
    /* Switch to non-blocking mode */
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    if (fcntl(stdin_fileno, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }

    /* Query for graphics protocol support */
    const uint32_t image_id = esp_random();
    fprintf(stdout, "\e_Gi=%lu,a=q,s=1,v=1;AAAAAA\e\\", image_id);
    fprintf(stdout, "\e[c");
    flush_write();

    /* Attempt to read response */
    char sequence[4] = {0};
    int timeout_ms = 500;
    const int retry_ms = 10;
    size_t read_bytes = 0;

    while (timeout_ms > 0 && read_bytes < sizeof(sequence) - 1) {
        usleep(retry_ms * 1000);
        timeout_ms -= retry_ms;

        char c;
        int cb = read(stdin_fileno, &c, 1);
        if (cb < 0 || (read_bytes == 0 && c != '\e')) {
            continue;
        }

        sequence[read_bytes++] = c;
    }

    if (timeout_ms > 0) {
        char c;
        while (read(stdin_fileno, &c, 1) > 0)
            ;
    }

    /* Restore previous mode */
    if (fcntl(stdin_fileno, F_SETFL, flags) == -1) {
        return -1;
    }

    if (strcmp(sequence, "\e_G") != 0)
        return -2;
    return 0;
}

int rt_cmd()
{
    int probe_result = probe_graphics();
    if (probe_result == -1) {
        int errnum = errno;
        ESP_LOGE(LOG_TAG, "Failed querying for graphics protocol support: %s",
                 strerror(errnum));
        return ESP_FAIL;
    } else if (probe_result == -2) {
        ESP_LOGE(LOG_TAG, "Graphics protocol not supported");
        return ESP_FAIL;
    }

    ESP_LOGI(LOG_TAG, "Graphics protocol supported");

    return ESP_OK;
}

void register_reaction_time_cmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "reaction_time",
        .help = "Can you react as fast as a neutrophil?",
        .func = &rt_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
