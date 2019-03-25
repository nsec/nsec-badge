#include <stdint.h>

#include "application.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"

#include "images/external/mines_background_bitmap.h"
#include "images/external/mines_controls_bitmap.h"
#include "images/external/mines_level_1_active_bitmap.h"
#include "images/external/mines_level_1_bitmap.h"
#include "images/external/mines_level_2_active_bitmap.h"
#include "images/external/mines_level_2_bitmap.h"
#include "images/external/mines_level_3_active_bitmap.h"
#include "images/external/mines_level_3_bitmap.h"
#include "images/external/mines_menu_bitmap.h"
#include "images/external/mines_splash_bitmap.h"

#define MINES_BUTTON_NONE 255

#define MINES_GAME_GOTO(new_state) p_state->current_state = new_state

#define MINES_GAME_STATE_BOOT 1
#define MINES_GAME_STATE_BOOT_SPLASH 2
#define MINES_GAME_STATE_MENU 3
#define MINES_GAME_STATE_CONTROLS 4
#define MINES_GAME_STATE_MANUAL 5
#define MINES_GAME_STATE_SELECT_LEVEL 6
#define MINES_GAME_STATE_INIT_GAME 7
#define MINES_GAME_STATE_DRAW_CANVAS 8
#define MINES_GAME_STATE_GAME 9
#define MINES_GAME_STATE_EXPLOSION 10
#define MINES_GAME_STATE_EXPLOSION_MSG 11
#define MINES_GAME_STATE_CLEARED 12
#define MINES_GAME_STATE_CLEARED_MSG 13
#define MINES_GAME_STATE_POST_MENU 14
#define MINES_GAME_STATE_FLAG 15
#define MINES_GAME_STATE_EXIT 255

static uint8_t mines_button_read_value = MINES_BUTTON_NONE;

typedef struct MinesGameState {
    uint8_t current_difficulty;
    uint8_t current_state;

    bool controls_rendered;
    uint8_t manual_position;
    bool manual_rendered;
    uint8_t menu_position;
    bool menu_rendered;
    uint8_t select_level_highlight;
    bool select_level_rendered;
} MinesGameState;

static void mines_buttons_handle(button_t button)
{
    mines_button_read_value = button;
}

static uint8_t mines_buttons_read()
{
    if (mines_button_read_value != MINES_BUTTON_NONE) {
        uint8_t value = mines_button_read_value;
        mines_button_read_value = MINES_BUTTON_NONE;

        return value;
    }

    return MINES_BUTTON_NONE;
}

static uint8_t mines_buttons_is_any_pushed()
{
    switch (mines_buttons_read()) {
    case BUTTON_BACK:
    case BUTTON_DOWN:
    case BUTTON_ENTER:
    case BUTTON_UP:
        return true;

    default:
        return false;
    }
}

static void mines_game_state_boot_handle(MinesGameState *p_state)
{
    display_draw_16bit_ext_bitmap(0, 0, &mines_splash_bitmap, 0);

    mines_buttons_read();
    MINES_GAME_GOTO(MINES_GAME_STATE_BOOT_SPLASH);
}

static void mines_game_state_boot_splash_handle(MinesGameState *p_state)
{
    if (mines_buttons_is_any_pushed()) {
        MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
    }
}

static void mines_game_state_cleared_handle(MinesGameState *p_state) {}
static void mines_game_state_cleared_msg_handle(MinesGameState *p_state) {}

static void mines_game_state_controls_handle(MinesGameState *p_state)
{
    if (!p_state->controls_rendered) {
        display_draw_16bit_ext_bitmap(0, 0, &mines_controls_bitmap, 0);
        p_state->controls_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->controls_rendered = false;
        MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
    }
}

static void mines_game_state_draw_canvas_handle(MinesGameState *p_state) {}
static void mines_game_state_explosion_handle(MinesGameState *p_state) {}
static void mines_game_state_explosion_msg_handle(MinesGameState *p_state) {}
static void mines_game_state_flag_handle(MinesGameState *p_state) {}
static void mines_game_state_game_handle(MinesGameState *p_state) {}
static void mines_game_state_init_game_handle(MinesGameState *p_state) {}

static void mines_game_state_manual_handle(MinesGameState *p_state)
{
    if (!p_state->manual_rendered) {
        const char strings[3][100] = {
            "   Your mind is a tool.\n   The more you use it,\n   the better "
            "you are\n   with it.",
            "   Schizophrenia and\n   dementia related to\n   the Beyond "
            "Reality are\n   rare, but possible.",
            "   Learn to avoid them\n   by playing this\n   fun game!"};

        display_draw_16bit_ext_bitmap(0, 0, &mines_background_bitmap, 0);

        gfx_set_cursor(0, 22);
        gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_WHITE);
        gfx_puts(strings[p_state->manual_position]);
        gfx_update();

        p_state->manual_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->manual_position++;
        p_state->manual_rendered = false;

        if (p_state->manual_position >= 3) {
            p_state->manual_position = 0;
            p_state->manual_rendered = false;

            MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
        }
    }
}

static void mines_game_state_menu_handle(MinesGameState *p_state)
{
    uint8_t cursor_x = 0, cursor_y = 0;

    uint8_t menu[] = {MINES_GAME_STATE_SELECT_LEVEL, MINES_GAME_STATE_CONTROLS,
                      MINES_GAME_STATE_MANUAL, MINES_GAME_STATE_EXIT};

    if (!p_state->menu_rendered) {
        display_draw_16bit_ext_bitmap(0, 0, &mines_menu_bitmap, 0);

        switch (p_state->menu_position) {
        case 0:
            cursor_x = 45;
            cursor_y = 11;
            break;

        case 1:
            cursor_x = 45;
            cursor_y = 30;
            break;

        case 2:
            cursor_x = 45;
            cursor_y = 47;
            break;

        case 3:
            cursor_x = 45;
            cursor_y = 65;
            break;
        }

        gfx_fill_triangle(cursor_x, cursor_y, cursor_x + 4, cursor_y + 2,
                          cursor_x, cursor_y + 4, DISPLAY_WHITE);
        gfx_update();

        p_state->menu_rendered = true;
    }

    switch (mines_buttons_read()) {
    case BUTTON_DOWN:
        p_state->menu_position =
            p_state->menu_position == 3 ? 0 : p_state->menu_position + 1;
        p_state->menu_rendered = false;
        break;

    case BUTTON_ENTER:
        p_state->menu_rendered = false;
        MINES_GAME_GOTO(menu[p_state->menu_position]);
        break;

    case BUTTON_UP:
        p_state->menu_position =
            p_state->menu_position == 0 ? 3 : p_state->menu_position - 1;
        p_state->menu_rendered = false;
        break;
    }
}

static void mines_game_state_post_menu_handle(MinesGameState *p_state) {}

static void mines_game_state_select_level_handle(MinesGameState *p_state)
{
    if (!p_state->select_level_rendered) {
        switch (p_state->current_difficulty) {
        case 0:
            display_draw_16bit_ext_bitmap(0, 0, &mines_level_1_bitmap, 0);
            break;

        case 1:
            display_draw_16bit_ext_bitmap(0, 0, &mines_level_2_bitmap, 0);
            break;

        case 2:
            display_draw_16bit_ext_bitmap(0, 0, &mines_level_3_bitmap, 0);
            break;
        }

        p_state->select_level_rendered = true;
    }

    p_state->select_level_highlight =
        (p_state->select_level_highlight + 1) % 30;

    switch (p_state->select_level_highlight) {
    case 15:
        switch (p_state->current_difficulty) {
        case 0:
            display_draw_16bit_ext_bitmap(11, 37, &mines_level_1_active_bitmap,
                                          0);
            break;

        case 1:
            display_draw_16bit_ext_bitmap(11, 37, &mines_level_2_active_bitmap,
                                          0);
            break;

        case 2:
            display_draw_16bit_ext_bitmap(11, 37, &mines_level_3_active_bitmap,
                                          0);
            break;
        }

        break;

    case 29:
        p_state->select_level_rendered = false;
        break;
    }

    switch (mines_buttons_read()) {
    case BUTTON_BACK:
        MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
        p_state->select_level_highlight = 0;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_DOWN:
        p_state->current_difficulty = p_state->current_difficulty == 2
                                          ? 0
                                          : p_state->current_difficulty + 1;
        p_state->select_level_highlight = 0;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_ENTER:
        MINES_GAME_GOTO(MINES_GAME_STATE_INIT_GAME);
        p_state->select_level_highlight = 0;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_UP:
        p_state->current_difficulty = p_state->current_difficulty == 0
                                          ? 2
                                          : p_state->current_difficulty - 1;
        p_state->select_level_highlight = 0;
        p_state->select_level_rendered = false;
        break;
    }
}

void mines_application(void (*service_device)())
{
    MinesGameState state = {.current_difficulty = 0,
                            .current_state = MINES_GAME_STATE_BOOT,
                            .controls_rendered = false,
                            .manual_position = 0,
                            .menu_rendered = false,
                            .menu_position = 0,
                            .menu_rendered = false,
                            .select_level_highlight = 0,
                            .select_level_rendered = false};

    nsec_controls_add_handler(mines_buttons_handle);

    while (application_get() == mines_application) {
        service_device();

        switch (state.current_state) {
        case MINES_GAME_STATE_BOOT:
            mines_game_state_boot_handle(&state);
            break;

        case MINES_GAME_STATE_BOOT_SPLASH:
            mines_game_state_boot_splash_handle(&state);
            break;

        case MINES_GAME_STATE_CLEARED:
            mines_game_state_cleared_handle(&state);
            break;

        case MINES_GAME_STATE_CLEARED_MSG:
            mines_game_state_cleared_msg_handle(&state);
            break;

        case MINES_GAME_STATE_CONTROLS:
            mines_game_state_controls_handle(&state);
            break;

        case MINES_GAME_STATE_DRAW_CANVAS:
            mines_game_state_draw_canvas_handle(&state);
            break;

        case MINES_GAME_STATE_EXPLOSION:
            mines_game_state_explosion_handle(&state);
            break;

        case MINES_GAME_STATE_EXIT:
            application_clear();
            break;

        case MINES_GAME_STATE_EXPLOSION_MSG:
            mines_game_state_explosion_msg_handle(&state);
            break;

        case MINES_GAME_STATE_FLAG:
            mines_game_state_flag_handle(&state);
            break;

        case MINES_GAME_STATE_GAME:
            mines_game_state_game_handle(&state);
            break;

        case MINES_GAME_STATE_INIT_GAME:
            mines_game_state_init_game_handle(&state);
            break;

        case MINES_GAME_STATE_MANUAL:
            mines_game_state_manual_handle(&state);
            break;

        case MINES_GAME_STATE_MENU:
            mines_game_state_menu_handle(&state);
            break;

        case MINES_GAME_STATE_POST_MENU:
            mines_game_state_post_menu_handle(&state);
            break;

        case MINES_GAME_STATE_SELECT_LEVEL:
            mines_game_state_select_level_handle(&state);
            break;

        default:
            state.current_state = MINES_GAME_STATE_EXIT;
        }
    }

    nsec_controls_suspend_handler(mines_buttons_handle);

    application_clear();
}
