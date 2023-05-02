#include "esp_console.h"
#include "esp_log.h"
#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#endif
#include "cmd_badge_mesh.h"

static const char *TAG = "badge_mesh";

static int stop_badge_mesh(int argc, char **argv) {
#ifdef CONFIG_BT_NIMBLE_ENABLED
    esp_err_t err;
    err = nimble_port_stop();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "oops nimble_port_stop errored out");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "nimble_port_stop called successfully");
#endif
    return ESP_OK;
}

void register_badge_mesh(void) {
    const esp_console_cmd_t badge_mesh_cmd = {
        .command = "stop_ble",
        .help = "Stop Bluetooth\n  Restart device to re-enable",
        .hint = NULL,
        .func = &stop_badge_mesh,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&badge_mesh_cmd));
}
