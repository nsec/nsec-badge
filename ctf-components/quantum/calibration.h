#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_console.h"
#include "ssd1306.h"
#include "driver/i2c.h"
#include "linenoise/linenoise.h"
#include <string>
#include <cstring>
#include <SmartLeds.h>
#include "board.hpp"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "badge_ssd1306_helper.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// Register the calibration command
void register_calibrate_cmd();

#ifdef __cplusplus
}
#endif

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_PIN 4
#define I2C_MASTER_SCL_PIN 5
#define I2C_MASTER_FREQ_HZ 100000

////

#define LED_PIN GPIO_NUM_8  // Pin connected to SK6812A data line
#define NUM_LEDS 18         // Number of LEDs in the strip

typedef struct {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
    uint8_t white; // Use this for SK6812A RGBW LEDs
} rgbw_t;