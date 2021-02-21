#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"
#include "rpg_control.h"

#include "rpg/MainCharacter.h"
#include "rpg/Scene.h"

void run_main_scene(void)
{
    rpg::Scene scene{"main", 1200, 1200};

    rpg::MainCharacter mc{};
    scene.add_character(&mc);

    scene.get_viewport().move_to_tile(13, 7);

    rpg_control_take(scene);
}
