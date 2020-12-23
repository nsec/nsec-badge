#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.c"
#include "uart.c"

void app_main(void)
{
    display_init();

    vTaskDelay(1000);
    xTaskCreate(uart_task, "uart_task", 1024, NULL, 10, NULL);

    while (1) {
        vTaskDelay(1000);
    }
}
