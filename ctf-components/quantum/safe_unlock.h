#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_console.h"
#include <SmartLeds.h>
#include "board.hpp"
#include <stdio.h>
#include "badge_ssd1306_helper.hpp"
#include <time.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <bitset>

#ifdef __cplusplus
extern "C" {
#endif

void register_qkd_safe_cmd();

#ifdef __cplusplus
}
#endif