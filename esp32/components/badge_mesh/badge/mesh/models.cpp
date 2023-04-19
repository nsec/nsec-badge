#include <esp_system.h>
#include <esp_log.h>

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"

#include "badge/mesh/config.h"
#include "badge/mesh/models.h"
#include "badge/mesh/ops.h"

static const char *TAG = "badge/mesh";

void mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param)
{
    uint32_t opcode;
    uint16_t addr;
    esp_err_t err;
    bool handled = false;

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT: {
        if (!param->model_operation.model || !param->model_operation.model->op ||
                !param->model_operation.ctx) {
            ESP_LOGE(TAG, "%s: model_operation parameter is NULL", __func__);
            return;
        }
        opcode = param->model_operation.opcode;
        addr = param->model_operation.ctx->addr;
        ESP_LOGV(TAG, "%s: opcode=0x%04" PRIx32 ", from 0x%02" PRIx16, __func__, opcode, addr);
        for(int i=0; mesh_callbacks[i].cb != 0; i++) {
            if(mesh_callbacks[i].op == opcode) {
                ESP_LOGV(TAG, "%s: Received message from client, opcode 0x%04" PRIx32, __func__, opcode);
                struct net_buf_simple buf = {
                    .data = param->model_operation.msg,
                    .len = param->model_operation.length,
                };

                handled = true;

                err = (mesh_callbacks[i].cb)(param->model_operation.model, param->model_operation.ctx, &buf);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "%s: Failed to handle message", __func__);
                    return;
                }
                break;
            }
        }
        if(!handled) {
            ESP_LOGW(TAG, "%s: unhandled message opcode 0x%04" PRIx32, __func__, param->model_operation.opcode);
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT: {
        // send completion event
        break;
    }
    default:
        ESP_LOGW(TAG, "unhandled custom model event %u", (int)event);
        break;
    }
}
