#include "esp_console.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buttons.h"
#include "cmd.h"
#include "graphics.h"
#include "rpg.h"

extern "C" void app_main(void)
{
    initialize_nvs();
    xTaskCreate(console_task, "console task", 4096, NULL, 3, NULL);
    nsec_buttons_init();
    graphics_start();

    run_main_scene();
}
