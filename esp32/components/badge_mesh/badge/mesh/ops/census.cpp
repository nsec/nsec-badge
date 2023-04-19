#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/host.h"
#include "badge/mesh/main.h"
#include "badge/mesh/ops/census.h"

static const char *TAG = "badge/mesh";

#if CONFIG_BADGE_MESH_ADMIN_COMMANDS

census_ctx_t census = {
    .in_progress = false,
};

esp_err_t send_census_request(census_response_cb_t resp_cb)
{
    esp_err_t err;

    if(census.in_progress) {
        ESP_LOGE(TAG, "%s: a census is already in progress", __func__);
        return ESP_FAIL;
    }

    if(!resp_cb) {
        ESP_LOGE(TAG, "%s: invalid arguments", __func__);
        return ESP_FAIL;
    }

	esp_fill_random(&census.uid, sizeof(census.uid));
    census.resp_cb = resp_cb;
    census.seen = 0;

    for(int i=0;i<census_device_type_t::count;i++)
        census.types_seen[i] = 0;

    census_request_data_t data = {
        .uid = census.uid,
    };

    err = mesh_client_send(NETWORK_GROUP_ADDR, OP_VND_CENSUS_REQUEST, (uint8_t *)&data, sizeof(data), false);
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed", __func__);
        return err;
    }

    ESP_LOGV(TAG, "Started census uid=0x%08lx", census.uid);
    census.in_progress = true;

    return ESP_OK;
}

esp_err_t census_response_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    census_response_data_t *data = (census_response_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    if(!census.in_progress || census.uid != data->uid) {
        ESP_LOGV(TAG, "%s: Ignoring (late?) census response from node=0x%04x uid=0x%08lx", __func__, ctx->addr, data->uid);
        return ESP_FAIL;
    }

    ESP_LOGV(TAG, "%s from node=0x%04x uid=0x%08lx", __func__, ctx->addr, data->uid);

    census.seen++;
    census.types_seen[data->device_type]++;
    census.resp_cb(ctx->addr, data->device_type);

    return ESP_OK;
}

void stop_census()
{
    if(census.in_progress) {
        ESP_LOGV(TAG, "Stopped census uid=0x%08lx seen=%u", census.uid, census.seen);
        census.in_progress = false;
    }
}

#endif /* CONFIG_BADGE_MESH_ADMIN_COMMANDS */

esp_err_t send_census_response(uint16_t dst, uint32_t uid)
{
    esp_err_t err;

    census_response_data_t data = {};

    memset(&data, 0, sizeof(data));
    data.uid = uid;
    data.device_type = CENSUS_DEVICE_TYPE;

    ESP_LOGV(TAG, "%s: dst=0x%04x uid=0x%08lx", __func__, dst, uid);

    err = mesh_client_send(dst, OP_VND_CENSUS_RESPONSE, (uint8_t *)&data, sizeof(data), false);
	if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed", __func__);
        return err;
    }

    return ESP_OK;
}

esp_err_t census_request_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    census_request_data_t *data = (census_request_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    ESP_LOGV(TAG, "%s from node=0x%04x uid=0x%08lx", __func__, ctx->addr, data->uid);

    /* try avoiding congesting network */
    vTaskDelay((esp_random() % CENSUS_FUZZ) / portTICK_PERIOD_MS);

    send_census_response(ctx->addr, data->uid);

    return ESP_OK;
}
