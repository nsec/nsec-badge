#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/host.h"
#include "badge/mesh/main.h"
#include "badge/mesh/ops/ping.h"

static const char *TAG = "badge/mesh";

ping_ctx_t ping;

esp_err_t send_pong(uint16_t addr, uint32_t uid)
{
    esp_err_t err;
    ping_data_t data = {
        .uid = uid,
    };

    ESP_LOGV(TAG, "Sending pong in response to node=0x%02x uid=0x%08lx", addr, uid);

    err = mesh_server_send(addr, OP_VND_PONG, (uint8_t *)&data, sizeof(data));
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Send pong failed (err %u)", __func__, err);
        return err;
    }

    return ESP_OK;
}

esp_err_t pong_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    ping_data_t *pong = (ping_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    if (ping.uid != pong->uid) {
        ESP_LOGE(TAG, "%s: Received pong response with incorrect uid", __func__);
		return ESP_OK;
	}

    xTaskNotify(ping.task, PING_NOTIFICATION_VALUE, eSetValueWithOverwrite);

    ESP_LOGV(TAG, "Received pong from node=0x%02x rssi=%d uid=0x%08lx",
        ctx->addr, ctx->recv_rssi, pong->uid);

    return ESP_OK;
}

esp_err_t send_ping(uint16_t dst, TaskHandle_t task)
{
    esp_err_t err;
    ping_data_t data = {
        .uid = esp_random(),
    };

    ping.uid = data.uid;
    ping.task = task;

    ESP_LOGV(TAG, "Sending ping to node=0x%02x uid=0x%08lx", dst, data.uid);

    err = mesh_client_send(dst, OP_VND_PING, (uint8_t *)&data, sizeof(data), true);
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Send ping failed", __func__);
        return err;
    }

    return ESP_OK;
}

esp_err_t ping_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    ping_data_t *ping = (ping_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    ESP_LOGV(TAG, "Received ping from node=0x%02x rssi=%d uid=0x%08lx",
        ctx->addr, ctx->recv_rssi, ping->uid);

    send_pong(ctx->addr, ping->uid);

    return ESP_OK;
}
