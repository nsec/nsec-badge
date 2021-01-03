#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "graphics.h"
#include "uart.h"

void app_main(void)
{
    graphics_start();
    vTaskDelay(10);

    xTaskCreate(uart_task, "uart_task", 1024, NULL, 10, NULL);

    while (1) {
        vTaskDelay(1000);
    }
}
