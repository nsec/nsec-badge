#pragma once

#include "rpg/Scene.h"

namespace rpg
{

enum class ControlExitAction {
    nothing,
    exit,
    restart,
};

ControlExitAction rpg_control_take(Scene &scene);

} // namespace rpg
