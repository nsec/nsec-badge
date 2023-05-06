#pragma once

#include <esp_system.h>

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODEL_SRV_ID            0x0000
#define MODEL_CLI_ID            0x0001

#define __ESP_BLE_MESH_SIG_MODEL(_id, _op, _pub, _user_data)        \
{                                                                   \
    .model_id = (_id),                                              \
    .pub = _pub,                                                    \
    .keys = { ESP_BLE_MESH_KEY_UNUSED, ESP_BLE_MESH_KEY_UNUSED, ESP_BLE_MESH_KEY_UNUSED }, \
    .groups = { ESP_BLE_MESH_ADDR_UNASSIGNED, ESP_BLE_MESH_ADDR_UNASSIGNED, ESP_BLE_MESH_ADDR_UNASSIGNED }, \
    .op = _op,                                                      \
    .user_data = _user_data,                                        \
}

#define __ESP_BLE_MESH_MODEL_CFG_SRV(srv_data)                              \
        __ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_CONFIG_SRV,          \
                  NULL, NULL, srv_data)

#define __ESP_BLE_MESH_MODEL_CFG_CLI(cli_data)                              \
        __ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_CONFIG_CLI,          \
                  NULL, NULL, cli_data)

#define __ESP_BLE_MESH_VENDOR_MODEL(_company, _id, _op, _pub, _user_data) \
{                                                                       \
    .vnd = {                                                            \
        .company_id = (_company),                                       \
        .model_id = (_id),                                              \
    },                                                                  \
    .pub = _pub,                                                        \
    .keys = { ESP_BLE_MESH_KEY_UNUSED, ESP_BLE_MESH_KEY_UNUSED, ESP_BLE_MESH_KEY_UNUSED }, \
    .groups = { ESP_BLE_MESH_ADDR_UNASSIGNED, ESP_BLE_MESH_ADDR_UNASSIGNED, ESP_BLE_MESH_ADDR_UNASSIGNED }, \
    .op = _op,                                                          \
    .user_data = _user_data,                                            \
}

void mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param);

#ifdef __cplusplus
}
#endif
