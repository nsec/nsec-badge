#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"
#include "console/console.h"
#include <badge_ssd1306_helper.hpp>

// Depending on which module you want to use, uncomment the appropriate line.
#include "qkd/qkd.h"
//#include "safe/safe.h"
//#include "codenames/codenames.h"





extern "C" void app_main(void) {

    fflush(stdout);

    /* Wait a few seconds before enabling the console. */
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Uncomment the module you want to use.
    //cmd_rf(0, NULL);
    //cmd_safe(0, NULL);
    cmd_qkd(0, NULL);

    /* Spin to prevent main task from exiting. */
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}