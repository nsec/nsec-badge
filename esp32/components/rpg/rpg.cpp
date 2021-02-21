#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"

extern "C" {
#include "graphics.h"
}

void run_scene(void)
{
    while (1) {
        vTaskDelay(1000);
    }
}
