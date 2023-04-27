#pragma once

#include "FX.h"
#include "FastLED.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

#define NUM_LEDS 21

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
    }

    const uint8_t _unlocked_mode[10] = {
        FX_MODE_BREATH,
        FX_MODE_RAINBOW,
        FX_MODE_RANDOM_COLOR,
        FX_MODE_CHASE_RAINBOW,
        FX_MODE_RAIN,
        FX_MODE_PRIDE_2015,
        FX_MODE_COLORWAVES,
        FX_MODE_SPARKLE,
        FX_MODE_RUNNING_COLOR,
        FX_MODE_RAINBOW_CYCLE
    };
    CRGB leds[NUM_LEDS];
    uint8_t _brightness;
    uint16_t _mode;
    uint16_t _public_mode;
    int _color;
    static bool is_on;
    static WS2812FX _ws2812fx;
    static TaskHandle_t _displayTaskHandle;

  public:
    NeoPixel(NeoPixel const &) = delete;
    void operator=(NeoPixel const &) = delete;

    static void displayPatterns(void *pvParameters)
    {
        while (true) {
            if(NeoPixel::is_on) {
                NeoPixel::_ws2812fx.service();
            }
            vTaskDelay(10 / portTICK_PERIOD_MS); /*10ms*/
        }
        vTaskDelete(NULL);
    }
    void init();
    void stop();
    void start();
    void setBrightness(uint8_t brightness);
    void setMode(uint8_t mode);
    void setPublicMode(uint8_t mode);
    void setColor(int color);
    uint8_t getBrightNess();
    uint16_t getPublicMode();
    uint16_t getMode();
    int getColor();
};
