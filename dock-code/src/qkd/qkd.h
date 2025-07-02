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

// Register the BB84 command
void register_qkd_cmd();
int cmd_qkd(int argc, char **argv);

#ifdef __cplusplus
}
#endif
