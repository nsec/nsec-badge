#include <esp_system.h>
#include <esp_log.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/ops.h"
#include "badge/mesh/ops/ping.h"
#include "badge/mesh/ops/census.h"
#include "badge/mesh/ops/info.h"
#include "badge/mesh/ops/neopixel.h"

static const char *TAG = "badge/mesh";

/*
    List all client messages, i.e. all message types that are sent in response to a prior client message.

    Use:
        ESP_BLE_MESH_MODEL_OP(message_type, min_length),

        `message_type`: one of the `OP_VND_` for this operation.
        `min_length`: the minimum required length for the message data.
*/
esp_ble_mesh_model_op_t vnd_cli_ops[] = {
    ESP_BLE_MESH_MODEL_OP(OP_VND_PONG, sizeof(ping_data_t)),
    ESP_BLE_MESH_MODEL_OP(OP_VND_INFO_RESPONSE, sizeof(info_response_data_t)),
    ESP_BLE_MESH_MODEL_OP_END,
};

/*
    List all server operations, i.e. all message types that are sent unsolicited from a node to another node.

    Use:
        ESP_BLE_MESH_MODEL_OP(message_type, min_length),

        `message_type`: one of the `OP_VND_` for this operation.
        `min_length`: the minimum required length for the message data.
*/
esp_ble_mesh_model_op_t vnd_srv_ops[] = {
    ESP_BLE_MESH_MODEL_OP(OP_VND_PING, sizeof(ping_data_t)),
    ESP_BLE_MESH_MODEL_OP(OP_VND_CENSUS_REQUEST, sizeof(census_request_data_t)),
    ESP_BLE_MESH_MODEL_OP(OP_VND_CENSUS_RESPONSE, sizeof(census_response_data_t)),
    ESP_BLE_MESH_MODEL_OP(OP_VND_INFO_REQUEST, 0),
    ESP_BLE_MESH_MODEL_OP(OP_VND_NEOPIXEL_SET, sizeof(neopixel_set_data_t)),
	ESP_BLE_MESH_MODEL_OP_END,
};

/*
    List pairs of client and server message types.

    For messages that require a response:
        { client_message_type, server_message_type },

    For messages that do not require a response:
        { client_message_type, 0 },
*/
esp_ble_mesh_client_op_pair_t op_pair[] = {
    { OP_VND_PING,              OP_VND_PONG             },
    { OP_VND_CENSUS_REQUEST,    NULL                    },
    { OP_VND_CENSUS_RESPONSE,   NULL                    },
    { OP_VND_INFO_REQUEST,      OP_VND_INFO_RESPONSE    },
};

esp_ble_mesh_client_t mesh_client = {
    .op_pair_size = ARRAY_SIZE(op_pair),
    .op_pair = op_pair,
};

/*
    Define callback methods for each possible OP_VND_* message types.
*/
mesh_callback_t mesh_callbacks[] = {
    { .op = OP_VND_PING, .cb = ping_received },
    { .op = OP_VND_PONG, .cb = pong_received },
    { .op = OP_VND_CENSUS_REQUEST, .cb = census_request_received },
#if CONFIG_BADGE_MESH_ADMIN_COMMANDS
    { .op = OP_VND_CENSUS_RESPONSE, .cb = census_response_received },
#endif /* CONFIG_BADGE_MESH_ADMIN_COMMANDS */
    { .op = OP_VND_INFO_REQUEST, .cb = info_request_received },
    { .op = OP_VND_INFO_RESPONSE, .cb = info_response_received },
    { .op = OP_VND_NEOPIXEL_SET, .cb = neopixel_set_received },

    { .op = 0, .cb = 0 }, // keep last
};
