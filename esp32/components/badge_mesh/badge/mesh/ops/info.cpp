#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/host.h"
#include "badge/mesh/main.h"
#include "badge/mesh/ops/info.h"

static const char *TAG = "badge/mesh";

info_request_ctx_t info_request;

esp_err_t send_info_response(uint16_t addr)
{
    esp_err_t err;
    info_response_data_t data = {};

    memcpy(&data.mac_addr, _device_address, sizeof(data.mac_addr));
    snprintf((char *)&data.name, sizeof(data.name), (char *)&badge_network_info.name);

    ESP_LOGV(TAG, "Sending info response to node=0x%02x", addr);

    err = mesh_server_send(addr, OP_VND_INFO_RESPONSE, (uint8_t *)&data, sizeof(data));
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed (err %u)", __func__, err);
        return err;
    }

    return ESP_OK;
}

esp_err_t info_response_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    info_response_data_t *data = (info_response_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    if (info_request.addr != ctx->addr) {
        ESP_LOGE(TAG, "%s: Received info response from another node", __func__);
		return ESP_OK;
	}

    memcpy(&info_request.response, data, sizeof(info_request.response));
    xTaskNotify(info_request.task, INFO_REQUEST_NOTIFICATION_VALUE, eSetValueWithOverwrite);

    ESP_LOGV(TAG, "Received info response from node=0x%02x rssi=%d", ctx->addr, ctx->recv_rssi);

    return ESP_OK;
}

esp_err_t send_info_request(uint16_t dst, TaskHandle_t task)
{
    esp_err_t err;

    info_request.addr = dst;
    info_request.task = task;

    ESP_LOGV(TAG, "Sending info request to node=0x%02x", dst);

    err = mesh_client_send(dst, OP_VND_INFO_REQUEST, NULL, 0, true);
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed", __func__);
        return err;
    }

    return ESP_OK;
}

esp_err_t info_request_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    ESP_LOGV(TAG, "Received info request from node=0x%02x rssi=%d", ctx->addr, ctx->recv_rssi);

    send_info_response(ctx->addr);

    return ESP_OK;
}
