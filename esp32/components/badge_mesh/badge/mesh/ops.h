#pragma once

#include <esp_system.h>

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

extern esp_ble_mesh_model_op_t vnd_cli_ops[];
extern esp_ble_mesh_model_op_t vnd_srv_ops[];
extern esp_ble_mesh_client_op_pair_t op_pair[];
extern esp_ble_mesh_client_t mesh_client;

typedef esp_err_t (* mesh_message_received_cb_t)(
    esp_ble_mesh_model_t *model,
    esp_ble_mesh_msg_ctx_t *ctx,
    struct net_buf_simple *buf
);

typedef struct mesh_callback {
    uint32_t op;
    mesh_message_received_cb_t cb;
} mesh_callback_t;

extern mesh_callback_t mesh_callbacks[];

#ifdef __cplusplus
}
#endif
