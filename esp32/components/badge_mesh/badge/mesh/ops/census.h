#pragma once

#include <esp_system.h>
#include "freertos/timers.h"

#include "esp_ble_mesh_defs.h"
#include "mesh_buf.h"

#include "badge/mesh/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_CENSUS_REQUEST                 0x03 /* must be unique */
#define OP_VND_CENSUS_REQUEST             ESP_BLE_MESH_MODEL_OP_3(OP_CENSUS_REQUEST, NSEC_COMPANY_ID)
#define OP_CENSUS_RESPONSE                0x04 /* must be unique */
#define OP_VND_CENSUS_RESPONSE            ESP_BLE_MESH_MODEL_OP_3(OP_CENSUS_RESPONSE, NSEC_COMPANY_ID)

#define CENSUS_DEFAULT_TIMEOUT_SECONDS    10
#define CENSUS_DEVICE_TYPE                (census_device_type_t)1 /* which type to reply with for this firmware */
#define CENSUS_FUZZ                       5000 /* try spreading responses over some time, to avoid congesting network */

typedef struct census_request_data {
    uint32_t uid; /* unique census ID, in case more than one is running at the same time */
} census_request_data_t;

enum census_device_type {
    screen = 0,
    badge,

    count,
};

typedef enum census_device_type census_device_type_t;

typedef struct census_response_data {
    uint32_t uid; /* unique census ID, in case more than one is running at the same time */
    census_device_type_t device_type;
} census_response_data_t;

typedef esp_err_t (* census_response_cb_t)(uint16_t addr, census_device_type_t device_type);

typedef struct census_ctx {
    uint32_t uid; /* uid of the current census */
    bool in_progress; /* true if currently in progress */
    census_response_cb_t resp_cb; /* called when a response is received */

    unsigned int seen;
    unsigned int types_seen[2];
} census_ctx_t;

extern census_ctx_t census;

/*
    Send the "census request" command to all nodes in the network group.

    'timeout': how long to wait for all responses to be received (default CENSUS_DEFAULT_TIMEOUT_SECONDS)
*/
esp_err_t send_census_request(census_response_cb_t resp_cb);
esp_err_t census_request_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

/*
    Stop census, if one is in progress.
*/
void stop_census();

/*
    Send the "census response" command to all nodes in the network group.

    'timeout': how long to wait for all responses to be received (default CENSUS_DEFAULT_TIMEOUT_SECONDS)
*/
esp_err_t send_census_response(uint16_t dst, uint32_t uid);
esp_err_t census_response_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

#ifdef __cplusplus
}
#endif
