#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/host.h"
#include "badge/mesh/main.h"
#include "badge/mesh/ops/neopixel.h"
#include <neopixel.h>

static const char *TAG = "badge/mesh";

neopixel_set_ctx_t neopixel_set = {
    .recv_at = 0,
    .recv_rssi = 0,
    .high_priority = false,
};

esp_err_t neopixel_set_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    neopixel_set_data_t *data = (neopixel_set_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    TickType_t recv_at = xTaskGetTickCount();
    TickType_t elapsed_ticks = recv_at - neopixel_set.recv_at;
    uint32_t elapsed_time_ms = (uint32_t)((elapsed_ticks * 1000) / configTICK_RATE_HZ);
    bool time_elapsed = neopixel_set.recv_at == 0 || elapsed_time_ms > (neopixel_set.data.time * 1000);
    bool stronger_signal = ctx->recv_rssi > neopixel_set.recv_rssi;
    bool high_priority = data->flags & NEOPIXEL_FLAG_HIGH_PRIORITY;
    bool same_sender = neopixel_set.recv_from == ctx->addr;
    bool should_obey = false;

    if (high_priority && !neopixel_set.high_priority) {
        // this message is high priority, but the previous wasn't, give priority to higher priority message
        should_obey = true;
    } else if(!high_priority && neopixel_set.high_priority) {
        /*
            This message isn't high priority, but the previous was, switch to the new command
            if the time is elapsed regardless of other conditions
        */
        should_obey = time_elapsed;
    } else {
        /*
            This message and the previous one have the same priority, switch to new command only if:
                - the previous command was from the same sender, or
                - the minimum time has elapsed, or
                - the new signal is stronger
        */
        should_obey = same_sender || time_elapsed || stronger_signal;
    }

    ESP_LOGV(TAG, "%s: time_elapsed=%s stronger_signal=%s high_priority=%s (old %s) same_sender=%s should_obey=%s", __func__,
        time_elapsed ? "yes" : "no",
        stronger_signal ? "yes" : "no",
        high_priority ? "yes" : "no",
        neopixel_set.high_priority ? "yes" : "no",
        same_sender ? "yes" : "no",
        should_obey ? "yes" : "no"
    );

    if(!should_obey) {
        ESP_LOGV(TAG, "%s: Ignoring neopixel_set command due to previous command", __func__);
		return ESP_OK;
    }

    neopixel_set.recv_at = recv_at;
    neopixel_set.recv_from = ctx->addr;
    neopixel_set.recv_rssi = ctx->recv_rssi;
    neopixel_set.high_priority = high_priority;
    memcpy(&neopixel_set.data, data, sizeof(neopixel_set.data));

    ESP_LOGV(TAG, "%s from node=0x%04x rssi=%d for=%usec", __func__, ctx->addr, ctx->recv_rssi, data->time);

    NeoPixel::getInstance().setColor((int)data->color);
    NeoPixel::getInstance().setBrightness(data->brightness);
    NeoPixel::getInstance().setPublicMode(data->mode);

    return ESP_OK;
}
