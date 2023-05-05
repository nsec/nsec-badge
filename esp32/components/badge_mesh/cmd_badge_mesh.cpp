#include "esp_console.h"
#include "esp_log.h"
#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "esp_ble_mesh_common_api.h"
#include "badge/mesh/config.h"
#endif
#include "cmd_badge_mesh.h"
#include "argtable3/argtable3.h"
#include "save.h"

static const char *TAG = "badge_mesh";

static struct {
    struct arg_str *action;
    struct arg_end *end;
} ble_args;


esp_err_t ble_stop() {
    if (!ble_hs_is_enabled()) {
        printf("Error: Bluetooth stack is not running.\n");
        return ESP_FAIL;
    }
#ifdef CONFIG_BT_NIMBLE_ENABLED
    printf("Stopping Bluetooth...\n");
    esp_err_t err;
    err = nimble_port_stop();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "oops nimble_port_stop errored out");
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "nimble_port_stop called successfully");
#endif
    printf("Bluetooth stopped.\n");
    return ESP_OK;
}
//nimble_port_run
void ble_status() {
    if (ble_hs_is_enabled()) {
        printf("Bluetooth stack is running.\n");
    } else {
        printf("Bluetooth stack is not running.\n");
    }
    printf("Address: 0x%04x\nMsg sent: %ld\nCompany ID: 0x%04x\nGroup address: 0x%04x\nSave state Bluetooth enabled: %s\n",
        badge_network_info.unicast_addr,
        bt_mesh.seq,
        NSEC_COMPANY_ID,
        badge_network_info.group_addr,
        Save::save_data.bluetooth_enabled ? "true" : "false"
    );

}

static int control_badge_mesh(int argc, char **argv) {
    if (arg_parse(argc, argv, (void **) &ble_args) != 0) {
        arg_print_errors(stderr, ble_args.end, argv[0]);
        return ESP_FAIL;
    }
    if (ble_args.action->count != 1) {
        printf("one of <enable, disable, stop, status> must be specified\n");
        return ESP_FAIL;
    }
    const char *action = ble_args.action->sval[0];
    esp_err_t ret = ESP_OK;
    if (strcmp(action, "enable") == 0)  {
        printf("Enabling Bluetooth and restarting...\n");
        Save::save_data.bluetooth_enabled = true;
        Save::write_save();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_restart();
    } else if (strcmp(action, "disable") == 0)  {
        printf("Disabling Bluetooth and restarting...\n");
        Save::save_data.bluetooth_enabled = false;
        Save::write_save();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_restart();
    } else if (strcmp(action, "stop") == 0)  {
        ret = ble_stop();
    } else if (strcmp(action, "status") == 0) {
        ble_status();
    }

    return ret;
}

void register_badge_mesh(void) {
    ble_args.action = arg_str0(NULL, NULL, "enable|disable|stop|status", NULL);
    //neopixel_args.all_modes = arg_lit0("a", "all", "unlock all modes, not just those unlocked by default.");
    ble_args.end = arg_end(2);

    const esp_console_cmd_t badge_mesh_cmd = {
        .command = "ble",
        .help = "Control Bluetooth\n  enable/disable: save state and restart\n  stop: no restart\n  status: show info",
        .hint = NULL,
        .func = &control_badge_mesh,
        .argtable = &ble_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&badge_mesh_cmd));
}
