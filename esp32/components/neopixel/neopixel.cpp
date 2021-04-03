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

WS2812FX NeoPixel::_ws2812fx;
TaskHandle_t NeoPixel::_displayTaskHandle;

void NeoPixel::init()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN_1>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(3, 1000);
    NeoPixel::_ws2812fx.init(NUM_LEDS, leds, false);
    setBrightness(25);
}

void NeoPixel::stop()
{
    if (NeoPixel::_displayTaskHandle) {
        vTaskDelete(NeoPixel::_displayTaskHandle);
    }
    NeoPixel::_ws2812fx.setBrightness(0);
    FastLED.setBrightness(0);
}

void NeoPixel::setColor(int color)
{
    if (color <= 0xffffff && color >= 0) {
        NeoPixel::_ws2812fx.setColor(0, color);
        _color = color;
    }
}

void NeoPixel::setBrightness(uint8_t brightness)
{
    if (brightness <= 0xff && brightness >= 0) {
        FastLED.setBrightness(brightness);
        NeoPixel::_ws2812fx.setBrightness(brightness);
        _brightness = brightness;
    }
}

void NeoPixel::setMode(uint16_t mode)
{
    if (NeoPixel::_displayTaskHandle) {
        vTaskDelete(NeoPixel::_displayTaskHandle);
    }
    NeoPixel::_ws2812fx.setMode(0, mode);
    xTaskCreate(&(NeoPixel::displayPatterns), "display_patterns", 1024, NULL, 5,
                &NeoPixel::_displayTaskHandle);
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

int NeoPixel::getColor()
{
    return _color;
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
