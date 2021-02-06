#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buttons.h"
#include "graphics.h"

void app_main(void)
{
    nsec_buttons_init();
    graphics_start();

    while (1) {
        vTaskDelay(1000);
    }
}
