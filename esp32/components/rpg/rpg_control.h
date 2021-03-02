#pragma once

#include "rpg/Scene.h"

namespace rpg
{

enum class ControlExitAction {
    nothing,
    exit,
    konami_code,
    restart,
};

ControlExitAction rpg_control_take(Scene &scene);

} // namespace rpg
