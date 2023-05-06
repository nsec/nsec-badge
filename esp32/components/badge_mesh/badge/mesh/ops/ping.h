#pragma once

#include <esp_system.h>

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#include "badge/mesh/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_PING                 0x00 /* must be unique */
#define OP_VND_PING             ESP_BLE_MESH_MODEL_OP_3(OP_PING, NSEC_COMPANY_ID)
#define OP_PONG                 0x01
#define OP_VND_PONG             ESP_BLE_MESH_MODEL_OP_3(OP_PONG, NSEC_COMPANY_ID)

/* value used to notify the task when pong is received */
#define PING_NOTIFICATION_VALUE (1 << 3)

typedef struct ping_data {
    uint32_t uid;
} ping_data_t;

typedef struct ping_ctx {
    uint32_t uid;
    TaskHandle_t task; /* handle of the task to notify when pong is received */
} ping_ctx_t;

/* we can only send one ping and wait for the response at one time */
extern ping_ctx_t ping;

esp_err_t send_ping(uint16_t addr, TaskHandle_t task);
esp_err_t ping_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

esp_err_t send_pong(uint16_t addr, uint32_t uid);
esp_err_t pong_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif
