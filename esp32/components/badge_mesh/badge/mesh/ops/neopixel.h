#pragma once

#include <esp_system.h>
#include "freertos/timers.h"

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#include "badge/mesh/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_NEOPIXEL_SET                   0x30 /* must be unique */
#define OP_VND_NEOPIXEL_SET               ESP_BLE_MESH_MODEL_OP_3(OP_NEOPIXEL_SET, NSEC_COMPANY_ID)

#define NEOPIXEL_SET_HIGH_PRIORITY (0 << 1) // admin patterns have higher priority

typedef struct neopixel_set_data {
    uint16_t time;
    uint8_t flags;
    uint8_t mode;
    uint8_t brightness;
    uint32_t color;
} neopixel_set_data_t;

typedef struct neopixel_set_ctx {
    TickType_t recv_at;
    uint16_t recv_from;
    int8_t recv_rssi;
    bool high_priority;
    neopixel_set_data_t data;
} neopixel_set_ctx_t;

/* Copy of the last received command */
extern neopixel_set_ctx_t neopixel_set;

esp_err_t neopixel_set_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif
