#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "graphics.h"

void app_main(void)
{
    graphics_start();

    while (1) {
        vTaskDelay(1000);
    }
}
