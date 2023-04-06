#include "neopixel.h"

#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>

#include "FX.h"
#include "FastLED.h"
#include "save.h"

#define DATA_PIN_1 33
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

static const char *TAG = "neopixel";

WS2812FX NeoPixel::_ws2812fx;
TaskHandle_t NeoPixel::_displayTaskHandle = NULL;
bool NeoPixel::is_on = false;

void NeoPixel::init()
{
    ESP_LOGV(TAG, "%s: starting", __func__);
    FastLED.addLeds<LED_TYPE, DATA_PIN_1, COLOR_ORDER>(_leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(3, 1000);
    NeoPixel::_ws2812fx.init(NUM_LEDS, _leds, false);

    if (Save::save_data.neopixel_is_on) {
        NeoPixel::start();
    }
}

void NeoPixel::start() {
    setColor(Save::save_data.neopixel_color);
    setBrightness(Save::save_data.neopixel_brightness);
    setPublicMode(Save::save_data.neopixel_mode);

    if(NeoPixel::_displayTaskHandle == NULL) {
        NeoPixel::is_on = true;
        ESP_LOGV(TAG, "%s: creating neopixel task", __func__);
        xTaskCreate(&(NeoPixel::displayPatterns), "display_patterns", 1024, NULL, 5, &NeoPixel::_displayTaskHandle);
    }
}

void NeoPixel::stop()
{
    NeoPixel::_ws2812fx.setBrightness(0);
    FastLED.setBrightness(0);
    vTaskDelay(100);
    NeoPixel::is_on = false;
}

void NeoPixel::setColor(int color)
{
    if (color <= 0xffffff && color >= 0) {
        NeoPixel::_ws2812fx.setColor(0, color);
        _color = color;
        Save::save_data.neopixel_color = color;
    }
}

void NeoPixel::setBrightness(uint8_t brightness)
{
    if (brightness <= 0xff && brightness >= 0) {
        FastLED.setBrightness(brightness);
        NeoPixel::_ws2812fx.setBrightness(brightness);
        _brightness = brightness;
        Save::save_data.neopixel_brightness = brightness;
    }
}

void NeoPixel::setPublicMode(uint8_t mode)
{
    NeoPixel::setMode(_unlocked_mode[mode % 10]);
    _public_mode = mode;
    Save::save_data.neopixel_mode = mode;
}

void NeoPixel::setMode(uint8_t mode)
{
    NeoPixel::_ws2812fx.setMode(0, mode);
    _mode = mode;
}

uint8_t NeoPixel::getBrightNess()
{
    return _brightness;
}

uint16_t NeoPixel::getMode()
{
    return _mode;
}

uint16_t NeoPixel::getPublicMode()
{
    return _public_mode;
}

int NeoPixel::getColor()
{
    return _color;
}

CRGB* NeoPixel::getFastLeds() {
    return _leds;
}

void neopixel_set_brightness(uint8_t brightness)
{
    NeoPixel::getInstance().setBrightness(brightness);
}
void neopixel_set_mode(uint16_t mode)
{
    NeoPixel::getInstance().setMode(mode);
}
void neopixel_set_color(int color)
{
    NeoPixel::getInstance().setColor(color);
}
