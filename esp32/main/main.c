#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buttons.h"
#include "graphics.h"
#include "rpg.h"

void app_main(void)
{
    nsec_buttons_init();
    graphics_start();

    run_main_scene();
}
