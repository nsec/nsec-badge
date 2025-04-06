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
#include "nvs.h"
#include "nvs_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

// Register the calibration command
void register_calibrate_cmd();

#ifdef __cplusplus
}
#endif