#include "rpg_action.h"

#include "rpg/MainCharacter.h"
#include "rpg/Viewport.h"

namespace rpg
{

using ACTION = ControlExitAction;

static ACTION handle_main_enter_action(Scene *scene, int tile_x, int tile_y)
{
    // Active area near the Door #6.
    if (tile_x >= 31 && tile_x <= 32 && tile_y >= 42 && tile_y <= 43) {
        return ACTION::exit;
    }

    return ACTION::nothing;
}

ACTION rpg_action_main_handle(Scene *scene, button_t button, void *extra_arg)
{
    ACTION ret = ACTION::nothing;

    struct timeval now;
    gettimeofday(&now, NULL);

    if (button == BUTTON_ENTER_RELEASE) {
        MainCharacter *mc = scene->get_main_character();

        // TODO: there is no method in the Viewport or in the Scene to do this
        // unit conversion.
        int tile_x = mc->get_scene_x() / DISPLAY_TILE_WIDTH;
        int tile_y = mc->get_scene_y() / DISPLAY_TILE_HEIGHT;

        ret = handle_main_enter_action(scene, tile_x, tile_y);
    }

    return ret;
}

} // namespace rpg
