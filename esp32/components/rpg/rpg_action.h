#pragma once

#include "rpg/Scene.h"
#include "rpg_control.h"

#include "buttons.h"

namespace rpg
{

/**
 * Character interactions handler for the main scene.
 */
ControlExitAction rpg_action_main_handle(Scene *scene, button_t button,
                                         void *extra_arg);

} // namespace rpg
