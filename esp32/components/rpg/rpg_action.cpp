#include "rpg_action.h"

#include "rpg/Coordinates.h"
#include "rpg/Viewport.h"
#include "rpg/characters/CharacterDuck.h"
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

static bool display_character_dialog(Character *character, Scene *scene,
                                     const char **dialog, int &start_line,
                                     int change)
{
    constexpr int dialog_position_x = 40;
    constexpr int dialog_position_y = 65;

    // Look for an array terminator and hope for the best. So much for memory
    // safety.
    int total_lines = 0;
    for (; dialog[total_lines][0] != '\0'; ++total_lines)
        assert(total_lines < 40 &&
               "Giving up looking for a terminator in a dialog array.");

    int line = start_line + change;
    if (line > total_lines - 6)
        line = total_lines - 6;

    if (line < 0)
        line = 0;

    if (change != 0 && line == start_line)
        return true;

    start_line = line;

    // Workaround the problem that the printed text bypasses the display
    // buffer. As the result, the buffer content does not change between two
    // draws of the dialog panel and nothing is actually sent to display.
    graphics_draw_from_library(LIBRARY_IMAGE_DIALOG_PANEL, 0, 1);
    graphics_draw_from_library(LIBRARY_IMAGE_DIALOG_PANEL, 0, 0);
    graphics_update_display();

    int color = 0x39c4;
    int x = dialog_position_x;
    int y = dialog_position_y;

    graphics_print_large(character->get_name(), x, y, color, &x, &y);
    graphics_print_large("_\n", x, y, color, &x, &y);

    x = dialog_position_x;
    for (; line < start_line + 6 && dialog[line][0] != '\0'; ++line)
        graphics_print_small(dialog[line], x, y, color, &x, &y);

    return true;
}

static ACTION handle_character_interaction(Character *character, Scene *scene)
{
    button_t button = BUTTON_NONE;
    bool displayed = false;
    int start_line = 0;

    scene->pause();
    scene->lock();

    const char **dialog = character->get_dialog();
    if (dialog) {
        // Make the dialog popup more legible before displaying anything.
        graphics_fadeout_display_buffer(50);

        // Display the dialog the first time and then go on to do more time
        // consuming things - the user will have something to read.
        displayed =
            display_character_dialog(character, scene, dialog, start_line, 0);
    }

    if (CharacterDuck::identify(character)) {
        // FIXME
    }

    while (displayed) {
        xQueueReceive(button_event_queue, &button, 10 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
            // Go on to break from the while loop.
            displayed = false;
            break;

        case BUTTON_DOWN:
            displayed = display_character_dialog(character, scene, dialog,
                                                 start_line, +5);
            break;

        case BUTTON_UP:
            displayed = display_character_dialog(character, scene, dialog,
                                                 start_line, -5);
            break;

        default:
            continue;
        }
    }

    scene->get_viewport().mark_for_full_refresh();
    scene->unlock();
    scene->unpause();

    return ACTION::nothing;
}

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
    auto mc = scene->get_main_character();
    auto coordinates = mc->get_coordinates();

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

    for (auto character : scene->get_characters()) {
        if (character == mc)
            continue;

        auto character_coordinates = character->get_coordinates();
        int delta_x = character_coordinates.x() - coordinates.x();
        int delta_y = character_coordinates.y() - coordinates.y();

        int distance_sq = (delta_x * delta_x) + (delta_y * delta_y);
        if (distance_sq < 600)
            return handle_character_interaction(character, scene);
    }

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
