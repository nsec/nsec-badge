#pragma once

#include "rpg/Scene.h"

namespace rpg
{

enum class ControlExitAction {
    nothing,
    exit,
    badge_info,
    konami_code,
    menu_led,
    menu_sound,
    menu_wifi,
    reverse_challenge,
    slideshow,
    restart,
};

ControlExitAction rpg_control_take(Scene &scene);

} // namespace rpg
