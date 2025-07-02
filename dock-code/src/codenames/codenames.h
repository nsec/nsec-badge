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
#include <badge_ssd1306_helper.hpp>
#include <bitset>
#include <cstring>  // For strcpy
#include "esp_system.h" // for reboot

#ifdef __cplusplus
extern "C" {
#endif

void register_rf_cmd();
int cmd_rf(int argc, char **argv);

#ifdef __cplusplus
}
#endif
