#include <string.h>

#include <esp_system.h>
#include <esp_log.h>
#include <esp_err.h>

#include "mesh_main.h"
#include "cfg_cli.h"
#include "settings.h"
#include "mesh.h"
#include "crypto.h"
#include "access.h"

#include "badge/mesh/network.h"

static const char *TAG = "badge/mesh";

int mesh_is_provisioned()
{
    return bt_mesh_is_provisioned();
}

int mesh_device_auto_enter_network(badge_network_info_t *info)
{
    const struct bt_mesh_comp *comp = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_elem *elem = NULL;
    struct bt_mesh_app_key::bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;
    struct bt_mesh_subnet *sub = NULL;
    int i, j, k;
    int err = 0;

    ESP_LOGV(TAG, "starting mesh_device_auto_enter_network");

    if (info == NULL || !BLE_MESH_ADDR_IS_UNICAST(info->unicast_addr) ||
            !BLE_MESH_ADDR_IS_GROUP(info->group_addr)) {
        return ESP_FAIL;
    }

    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_NODE);

    /* The device becomes a node and enters the network */
    err = bt_mesh_provision(info->net_key, info->net_idx, info->flags, info->iv_index,
                            info->unicast_addr, info->dev_key);
    if (err) {
        ESP_LOGE(TAG, "bt_mesh_provision() failed (err %d)", err);
        return err;
    }

    /* Adds application key to device */
    sub = bt_mesh_subnet_get(info->net_idx);
    if (!sub) {
        ESP_LOGE(TAG, "Invalid NetKeyIndex 0x%04x", info->net_idx);
        return ESP_FAIL;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx == BLE_MESH_KEY_UNUSED) {
            break;
        }
    }
    if (i == ARRAY_SIZE(bt_mesh.app_keys)) {
        ESP_LOGE(TAG, "Failed to allocate AppKey, 0x%04x", info->app_idx);
        return ESP_FAIL;
    }

    keys = sub->kr_flag ? &key->keys[1] : &key->keys[0];

    if (bt_mesh_app_id(info->app_key, &keys->id)) {
        ESP_LOGE(TAG, "Failed to calculate AID, 0x%04x", info->app_idx);
        return ESP_FAIL;
    }

    key->net_idx = info->net_idx;
    key->app_idx = info->app_idx;
    memcpy(keys->val, info->app_key, 16);

    /* Binds AppKey with all non-config models, adds group address to all these models */
    comp = bt_mesh_comp_get();
    if (!comp) {
        ESP_LOGE(TAG, "Invalid composition data");
        return ESP_FAIL;
    }

    for (i = 0; i < comp->elem_count; i++) {
        elem = &comp->elem[i];
        for (j = 0; j < elem->model_count; j++) {
            model = &elem->models[j];
            if (model->id == BLE_MESH_MODEL_ID_CFG_SRV ||
                    model->id == BLE_MESH_MODEL_ID_CFG_CLI) {
                continue;
            }
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = info->app_idx;
                    break;
                }
            }
            for (k = 0; k < ARRAY_SIZE(model->groups); k++) {
                if (model->groups[k] == BLE_MESH_ADDR_UNASSIGNED) {
                    model->groups[k] = info->group_addr;
                    break;
                }
            }
        }
        for (j = 0; j < elem->vnd_model_count; j++) {
            model = &elem->vnd_models[j];
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = info->app_idx;
                    break;
                }
            }
            for (k = 0; k < ARRAY_SIZE(model->groups); k++) {
                if (model->groups[k] == BLE_MESH_ADDR_UNASSIGNED) {
                    model->groups[k] = info->group_addr;
                    break;
                }
            }
        }
    }

    return ESP_OK;
}
