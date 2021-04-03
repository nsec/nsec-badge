#include "neopixel.h"

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>

#include "FX.h"
#include "FastLED.h"

#define NUM_LEDS 15
#define DATA_PIN_1 33
#define LED_TYPE WS2811
#define COLOR_ORDER RGB

CRGB leds1[NUM_LEDS];
TaskHandle_t displayTaskHandle = NULL;
static WS2812FX ws2812fx;

void neopixel_init()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN_1>(leds1, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(3, 1000);
    ws2812fx.init(NUM_LEDS, leds1, false);
}

void neopixel_stop()
{
    if (displayTaskHandle) {
        vTaskDelete(displayTaskHandle);
    }
    ws2812fx.setBrightness(0);
    FastLED.setBrightness(0);
}

void neopixel_set_color(int color)
{
    ws2812fx.setColor(0, color);
}

void neopixel_set_brightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
    ws2812fx.setBrightness(brightness);
}

static void display_patterns(void *pvParameters)
{
    while (true) {
        ws2812fx.service();
        vTaskDelay(10 / portTICK_PERIOD_MS); /*10ms*/
    }
}

void neopixel_set_mode(uint16_t mode)
{
    if (displayTaskHandle) {
        vTaskDelete(displayTaskHandle);
    }
    ws2812fx.setMode(0, mode);
    xTaskCreate(&display_patterns, "display_patterns", 4000, NULL, 5,
                &displayTaskHandle);
}
