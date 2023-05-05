#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>

#include "esp_ble_mesh_defs.h"

#include "badge/mesh/host.h"
#include "badge/mesh/main.h"
#include "badge/mesh/ops/realtimeled.h"
#include <neopixel.h>
#include "FastLED.h"
#include "esp_timer.h"

#define RT_TIMER_FACTOR 1000000

static const char *TAG = "badge/mesh/rtled";
static bool rtled_is_running = false;

esp_timer_handle_t rtled_timer;

static void rtled_timer_cycle(void *arg) {
    ESP_LOGI(TAG, "timer cycle, time since boot: %lld us", esp_timer_get_time());
    rtled_is_running = false;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    NeoPixel::getInstance().start();
    ESP_LOGW("NSEC", "REAL TIME LED MODE DISABLED");
}

void rtled_init(uint16_t start) {
    ESP_LOGW("NSEC", "REAL TIME LED MODE ENABLED");
    rtled_is_running = true;
    NeoPixel::getInstance().stop();
    FastLED.clear();
    FastLED.setBrightness(125);
    FastLED.show();
    const esp_timer_create_args_t rtled_timer_args = { .callback = rtled_timer_cycle };
    ESP_ERROR_CHECK(esp_timer_create(&rtled_timer_args, &rtled_timer));
    ESP_ERROR_CHECK(esp_timer_start_once(rtled_timer, start * RT_TIMER_FACTOR));
}

CRGB decodeColor(uint8_t encodedColor) {
    uint8_t r = (encodedColor >> 5) & 0x07; // shift right and mask to get 3 bits
    uint8_t g = (encodedColor >> 2) & 0x07;
    uint8_t b = encodedColor & 0x03; // mask to get 2 bits
    return CRGB(r << 5, g << 5, b << 6); // shift left to restore original values
}

esp_err_t realtimeled_received(esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    realtimeled_data_t *data = (realtimeled_data_t *)buf->data;
	if (ctx->addr == model->element->element_addr) {
        ESP_LOGV(TAG, "%s: Ignoring message from self", __func__);
		return ESP_OK;
	}

    ESP_LOGI(TAG, "%s from node=0x%04x start=%d ledids:0x%lu color:0x%u brightness:%d", __func__, ctx->addr, data->start, data->ledids, data->color, data->brightness);

    if (!rtled_is_running && data->start) {
        rtled_init(data->start);
    } 
    
    if (rtled_is_running) {
        CRGB *leds = NeoPixel::getInstance().getFastLeds();
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        CRGB color = decodeColor(data->color);
        uint32_t _ledids = data->ledids;
        for (int i = 0; i < NUM_LEDS; i++) {
            if(_ledids & (1<<i))
                leds[i] = color;
        }
        FastLED.setBrightness(data->brightness);
        FastLED.show();
    }
    return ESP_OK;
}
