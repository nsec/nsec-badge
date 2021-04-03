#include "rpg_action.h"

#include "rpg/Coordinates.h"
#include "rpg/Viewport.h"
#include "rpg/characters/MainCharacter.h"

namespace rpg
{

using ACTION = ControlExitAction;

enum class Oversign {
    badger,
    customs,
    hut,
    port,
    quack,
};

static ACTION handle_show_oversign(Oversign oversign, Scene *scene)
{
    scene->pause();
    scene->lock();

    switch (oversign) {
    case Oversign::badger:
        graphics_draw_from_library(LIBRARY_IMAGE_OVERSIGN_BADGER, 0, 0);
        break;

    case Oversign::customs:
        graphics_draw_from_library(LIBRARY_IMAGE_OVERSIGN_CUSTOMS, 0, 0);
        break;

    case Oversign::hut:
        graphics_draw_from_library(LIBRARY_IMAGE_OVERSIGN_HUT, 0, 0);
        break;

    case Oversign::port:
        graphics_draw_from_library(LIBRARY_IMAGE_OVERSIGN_PORT, 0, 0);
        break;

    case Oversign::quack:
        graphics_draw_from_library(LIBRARY_IMAGE_OVERSIGN_QUACK, 0, 0);
        break;
    }

    graphics_update_display();

    // Block control until the user breaks out by pressing back.
    button_t button = BUTTON_NONE;
    do {
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);
    } while (button != BUTTON_BACK_RELEASE);

    scene->get_viewport().mark_for_full_refresh();
    scene->unlock();
    scene->unpause();

    return ACTION::nothing;
}

static ACTION handle_main_enter_action(Scene *scene)
{
    auto coordinates = scene->get_main_character()->get_coordinates();

    // Active area near the Door #6.
    if (coordinates.within_tile(31, 42, 32, 43))
        return ACTION::exit;

    if (coordinates.within_xy(100, 360, 140, 390))
        return handle_show_oversign(Oversign::hut, scene);

    if (coordinates.within_xy(200, 1010, 240, 1035))
        return handle_show_oversign(Oversign::badger, scene);

    if (coordinates.within_xy(485, 985, 525, 1010))
        return handle_show_oversign(Oversign::customs, scene);

    if (coordinates.within_xy(485, 530, 525, 550))
        return handle_show_oversign(Oversign::quack, scene);

    if (coordinates.within_xy(1000, 675, 1040, 700))
        return handle_show_oversign(Oversign::port, scene);

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
        ret = handle_main_enter_action(scene);
    }

    return ret;
}

} // namespace rpg
