#pragma once

#include "rpg/Scene.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace rpg
{

enum class ControlExitAction {
    nothing,
    exit,
    badge_info,
    hall_of_fame,
    konami_code,
    menu_led,
    menu_sound,
    menu_wifi,
    reverse_challenge,
    restart,
};

struct RpgControlDevice {
    Scene *scene;
    TaskHandle_t task_animation_step;
    TaskHandle_t task_main_character;
    TaskHandle_t task_render;
    TaskHandle_t task_statusbar;
    ControlExitAction exit_action;
    int fps_counter;
};

void rpg_control_take(RpgControlDevice &control_device);

} // namespace rpg
