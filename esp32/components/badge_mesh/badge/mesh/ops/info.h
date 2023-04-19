#pragma once

#include <esp_system.h>

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#include "badge/mesh/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_INFO_REQUEST                 0x06 /* must be unique */
#define OP_VND_INFO_REQUEST             ESP_BLE_MESH_MODEL_OP_3(OP_INFO_REQUEST, NSEC_COMPANY_ID)
#define OP_INFO_RESPONSE                0x07
#define OP_VND_INFO_RESPONSE            ESP_BLE_MESH_MODEL_OP_3(OP_INFO_RESPONSE, NSEC_COMPANY_ID)

/* value used to notify the task when response is received */
#define INFO_REQUEST_NOTIFICATION_VALUE (1 << 4)

typedef struct info_response_data {
    uint8_t mac_addr[6];
    char name[BADGE_NAME_LEN];
} info_response_data_t;

typedef struct info_request_ctx {
    uint16_t addr; /* address of the node being queried */
    TaskHandle_t task; /* handle of the task to notify when response is received */

    info_response_data_t response; /* filled with the response received from the node */
} info_request_ctx_t;

/* we can only send one ping and wait for the response at one time */
extern info_request_ctx_t info_request;

esp_err_t send_info_request(uint16_t addr, TaskHandle_t task);
esp_err_t info_request_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

esp_err_t send_info_response(uint16_t addr);
esp_err_t info_response_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif
