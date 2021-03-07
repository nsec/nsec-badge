#include "rpg_action.h"

#include "rpg/Coordinates.h"
#include "rpg/Viewport.h"
#include "rpg/characters/MainCharacter.h"

namespace rpg
{

using ACTION = ControlExitAction;

static ACTION handle_main_enter_action(Scene *scene,
                                       GlobalCoordinates coordinates)
{
    // Active area near the Door #6.
    if (coordinates.within_tile(31, 42, 32, 43))
        return ACTION::exit;

    return ACTION::nothing;
}

static bool handle_main_konami_code(Scene *scene, button_t button,
                                    struct timeval &time)
{
    static uint32_t konami_code = 0;
    static uint32_t konami_code_timer = 0;

    unsigned int time_usec =
        (int64_t)time.tv_sec * 1000000L + (int64_t)time.tv_usec;

    if (time_usec - konami_code_timer > 1000000L)
        konami_code = 0;

    konami_code_timer = time_usec;

    switch (button) {
    case BUTTON_BACK:
        konami_code = (konami_code << 3) + 0b001;
        break;

    case BUTTON_DOWN:
        konami_code = (konami_code << 3) + 0b010;
        break;

    case BUTTON_ENTER:
        konami_code = (konami_code << 3) + 0b011;
        break;

    case BUTTON_LEFT:
        konami_code = (konami_code << 3) + 0b100;
        break;

    case BUTTON_RIGHT:
        konami_code = (konami_code << 3) + 0b101;
        break;

    case BUTTON_UP:
        konami_code = (konami_code << 3) + 0b110;
        break;

    default:
        break;
    }

    return konami_code == 910842187;
}

ACTION rpg_action_main_handle(Scene *scene, button_t button, void *extra_arg)
{
    ACTION ret = ACTION::nothing;

    struct timeval now;
    gettimeofday(&now, NULL);

    if (handle_main_konami_code(scene, button, now))
        return ACTION::konami_code;

    if (button == BUTTON_ENTER_RELEASE) {
        auto coordinates = scene->get_main_character()->get_coordinates();
        ret = handle_main_enter_action(scene, coordinates);
    }

    return ret;
}

} // namespace rpg
