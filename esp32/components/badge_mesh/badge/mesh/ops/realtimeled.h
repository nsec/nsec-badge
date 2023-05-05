#pragma once

#include <esp_system.h>
#include "freertos/timers.h"

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#include "badge/mesh/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_REALTIMELED                   0x77 /* must be unique */
#define OP_VND_REALTIMELED               ESP_BLE_MESH_MODEL_OP_3(OP_REALTIMELED, NSEC_COMPANY_ID)

typedef struct realtimeled_data {
    uint16_t start;
    uint32_t ledids;
    uint8_t color;
    uint8_t brightness;
 } realtimeled_data_t;

esp_err_t realtimeled_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif
