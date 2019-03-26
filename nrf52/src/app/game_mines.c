#include <stdint.h>
#include <string.h>

#include <app_timer.h>

#include "application.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "random.h"

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

#define MINES_GAME_CELL2LIST(x, y) y * p_state->field_width + x
#define MINES_GAME_LIST2CELL_X(value) value % p_state->field_width
#define MINES_GAME_LIST2CELL_Y(value) value / p_state->field_width

#define MINES_GAME_FIELD_CELL 8
#define MINES_GAME_FIELD_SIDEBAR_POSITION 88
#define MINES_GAME_FIELD_SIZE (p_state->field_width * p_state->field_height)

#define MINES_GAME_GOTO(new_state) p_state->current_state = new_state

#define MINES_GAME_LIST_LIMIT (11 * 8 - 22)

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

#define MINES_TIMER_INTERVAL 10

#define MINES_TIMER_CANCEL(timer) p_state->timer_##timer = 0
#define MINES_TIMER_ENDED(timer) p_state->timer_##timer == 0
#define MINES_TIMER_START(timer, end)                                          \
    p_state->timer_##timer = end / MINES_TIMER_INTERVAL
#define MINES_TIMER_TICK(timer) p_state->timer_##timer--

APP_TIMER_DEF(mines_timer_id);

static uint8_t mines_button_read_value = MINES_BUTTON_NONE;

static uint8_t mines_difficulty_levels[3][3] = {
    {5, 5, 7}, {7, 7, 11}, {11, 8, 22}};
//                           ^  ^  ^
//               field width +  |  |
//              field height -  +  |
//           number of mines -  -  +

typedef struct MinesGameState {
    uint8_t opened[MINES_GAME_LIST_LIMIT];
    uint8_t mines[MINES_GAME_LIST_LIMIT];
    uint8_t holds[MINES_GAME_LIST_LIMIT];
    uint8_t flags[MINES_GAME_LIST_LIMIT];

    uint8_t current_difficulty;
    uint8_t current_state;
    bool cursor_activated;
    uint8_t cursor_x;
    uint8_t cursor_y;
    uint8_t field_height;
    uint8_t field_width;
    uint8_t flags_placed;
    uint8_t holds_placed;
    uint8_t mines_total;
    uint8_t opened_count;

    bool controls_rendered;
    uint8_t manual_position;
    bool manual_rendered;
    uint8_t menu_position;
    bool menu_rendered;
    bool select_level_highlight;
    bool select_level_rendered;

    uint16_t timer_button;
    uint16_t timer_cursor;
    uint16_t timer_flags_warning;
    uint16_t timer_select_level_highlight;
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

static bool mines_is_in_list(uint8_t list[], uint8_t tail, uint8_t value)
{
    for (uint8_t i = 0; i < tail; i++) {
        if (list[i] == value) {
            return true;
        }
    }

    return false;
}

static void mines_add_to_list(uint8_t list[], uint8_t *tail, uint8_t value)
{
    if (!mines_is_in_list(list, *tail, value)) {
        list[*tail] = value;
        (*tail)++;
    }
}

static void mines_remove_from_list(uint8_t list[], uint8_t *tail, uint8_t value)
{
    uint8_t i = 0, t = *tail;

    for (; i < t; i++) {
        if (list[i] == value) {
            for (uint8_t j = i + 1; j < t; j++) {
                list[j - 1] = list[j];
            }

            (*tail)--;
        }
    }
}

static bool mines_game_has_flag_at(MinesGameState *p_state, uint8_t value)
{
    return mines_is_in_list(p_state->flags, p_state->flags_placed, value);
}

static bool mines_game_has_hold_at(MinesGameState *p_state, uint8_t value)
{
    return mines_is_in_list(p_state->holds, p_state->holds_placed, value);
}

static bool mines_game_has_mine_at(MinesGameState *p_state, uint8_t value)
{
    return mines_is_in_list(p_state->mines, p_state->mines_total, value);
}

static bool mines_game_is_opened(MinesGameState *p_state, uint8_t value)
{
    return mines_is_in_list(p_state->opened, p_state->opened_count, value);
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

static void mines_game_state_init_game_handle(MinesGameState *p_state)
{
    uint8_t value;

    memset(p_state->flags, 255, MINES_GAME_LIST_LIMIT);
    memset(p_state->holds, 255, MINES_GAME_LIST_LIMIT);
    memset(p_state->mines, 255, MINES_GAME_LIST_LIMIT);
    memset(p_state->opened, 255, MINES_GAME_LIST_LIMIT);

    p_state->flags_placed = 0;
    p_state->holds_placed = 0;
    p_state->opened_count = 0;

    p_state->field_width =
        mines_difficulty_levels[p_state->current_difficulty][0];
    p_state->field_height =
        mines_difficulty_levels[p_state->current_difficulty][1];
    p_state->mines_total =
        mines_difficulty_levels[p_state->current_difficulty][2];

    for (uint8_t i = 0; i < p_state->mines_total; i++) {
        do {
            value = nsec_random_get_byte(MINES_GAME_FIELD_SIZE - 1);
        } while (mines_game_has_mine_at(p_state, value));

        p_state->mines[i] = value;
    }

    do {
        value = nsec_random_get_byte(MINES_GAME_FIELD_SIZE - 1);
    } while (mines_game_has_mine_at(p_state, value));

    p_state->cursor_activated = 0;
    p_state->cursor_x = MINES_GAME_LIST2CELL_X(value);
    p_state->cursor_y = MINES_GAME_LIST2CELL_Y(value);

    MINES_TIMER_CANCEL(button);
    MINES_TIMER_CANCEL(cursor);
    MINES_TIMER_CANCEL(flags_warning);

    MINES_GAME_GOTO(MINES_GAME_STATE_DRAW_CANVAS);
}

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
        MINES_TIMER_START(select_level_highlight, 300);

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

    if (MINES_TIMER_ENDED(select_level_highlight)) {
        if (p_state->select_level_highlight) {
            p_state->select_level_highlight = false;
            p_state->select_level_rendered = false;
        } else {
            p_state->select_level_highlight = true;

            switch (p_state->current_difficulty) {
            case 0:
                display_draw_16bit_ext_bitmap(11, 37,
                                              &mines_level_1_active_bitmap, 0);
                break;

            case 1:
                display_draw_16bit_ext_bitmap(11, 37,
                                              &mines_level_2_active_bitmap, 0);
                break;

            case 2:
                display_draw_16bit_ext_bitmap(11, 37,
                                              &mines_level_3_active_bitmap, 0);
                break;
            }

            MINES_TIMER_START(select_level_highlight, 300);
        }
    }

    switch (mines_buttons_read()) {
    case BUTTON_BACK:
        MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
        p_state->select_level_highlight = false;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_DOWN:
        p_state->current_difficulty = p_state->current_difficulty == 2
                                          ? 0
                                          : p_state->current_difficulty + 1;
        p_state->select_level_highlight = false;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_ENTER:
        MINES_GAME_GOTO(MINES_GAME_STATE_INIT_GAME);
        p_state->select_level_highlight = false;
        p_state->select_level_rendered = false;
        break;

    case BUTTON_UP:
        p_state->current_difficulty = p_state->current_difficulty == 0
                                          ? 2
                                          : p_state->current_difficulty - 1;
        p_state->select_level_highlight = false;
        p_state->select_level_rendered = false;
        break;
    }
}

static void mines_timer_handle(void *p_context)
{
    MinesGameState *p_state = (MinesGameState *)p_context;

    if (!MINES_TIMER_ENDED(button))
        MINES_TIMER_TICK(button);

    if (!MINES_TIMER_ENDED(cursor))
        MINES_TIMER_TICK(cursor);

    if (!MINES_TIMER_ENDED(flags_warning))
        MINES_TIMER_TICK(flags_warning);

    if (!MINES_TIMER_ENDED(select_level_highlight))
        MINES_TIMER_TICK(select_level_highlight);
}

void mines_application(void (*service_device)())
{
    MinesGameState state = {.controls_rendered = false,
                            .current_difficulty = 0,
                            .current_state = MINES_GAME_STATE_BOOT,
                            .cursor_activated = false,
                            .cursor_x = 0,
                            .cursor_y = 0,
                            .field_height = 0,
                            .field_width = 0,
                            .flags = {},
                            .flags_placed = 0,
                            .holds = {},
                            .holds_placed = 0,
                            .manual_position = 0,
                            .menu_position = 0,
                            .menu_rendered = false,
                            .menu_rendered = false,
                            .mines = {},
                            .mines_total = 0,
                            .opened = {},
                            .opened_count = 0,
                            .select_level_highlight = false,
                            .select_level_rendered = false};

    app_timer_create(&mines_timer_id, APP_TIMER_MODE_REPEATED,
                     mines_timer_handle);
    app_timer_start(mines_timer_id, APP_TIMER_TICKS(MINES_TIMER_INTERVAL),
                    &state);
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

    app_timer_stop(mines_timer_id);
    nsec_controls_suspend_handler(mines_buttons_handle);

    application_clear();
}
