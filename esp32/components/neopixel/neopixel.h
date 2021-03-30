#pragma once

#include "FX.h"
#include "FastLED.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#define NUM_LEDS 15

class NeoPixel
{
  public:
    static NeoPixel &getInstance()
    {
        static NeoPixel instance;
        return instance;
    }

  private:
    NeoPixel()
    {
        NeoPixel::init();
    }

    void init();

    CRGB leds[NUM_LEDS];
    uint8_t _brightness;
    uint16_t _mode;
    int _color;
    static WS2812FX _ws2812fx;
    static TaskHandle_t _displayTaskHandle;

  public:
    NeoPixel(NeoPixel const &) = delete;
    void operator=(NeoPixel const &) = delete;

    static void displayPatterns(void *pvParameters)
    {
        while (true) {
            NeoPixel::_ws2812fx.service();
            vTaskDelay(10 / portTICK_PERIOD_MS); /*10ms*/
        }
    }
    void stop();
    void setBrightness(uint8_t brightness);
    void setMode(uint16_t mode);
    void setColor(int color);
    uint8_t getBrightNess();
    uint16_t getMode();
    int getColor();
};
