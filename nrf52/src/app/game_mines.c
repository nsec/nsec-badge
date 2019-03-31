#include <stdint.h>
#include <string.h>

#include <app_timer.h>
#include <nrf_delay.h>

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
#include "images/external/mines_message_boom_bitmap.h"
#include "images/external/mines_message_cleared_bitmap.h"
#include "images/external/mines_pattern_1_bitmap.h"
#include "images/external/mines_pattern_2_bitmap.h"
#include "images/external/mines_pattern_3_bitmap.h"
#include "images/external/mines_pattern_4_bitmap.h"
#include "images/external/mines_pattern_5_bitmap.h"
#include "images/external/mines_pattern_6_bitmap.h"
#include "images/external/mines_pattern_7_bitmap.h"
#include "images/external/mines_pattern_8_bitmap.h"
#include "images/external/mines_pattern_blank_bitmap.h"
#include "images/external/mines_pattern_boom_bitmap.h"
#include "images/external/mines_pattern_cleared_bitmap.h"
#include "images/external/mines_pattern_flag_bitmap.h"
#include "images/external/mines_pattern_hold_bitmap.h"
#include "images/external/mines_pattern_mine_bitmap.h"
#include "images/external/mines_sidebar_bitmap.h"
#include "images/external/mines_sidebar_warning_bitmap.h"
#include "images/external/mines_splash_bitmap.h"

#define MINES_BUTTON_NONE 255

#define MINES_GAME_CELL2LIST(x, y) y * p_state->field_width + x
#define MINES_GAME_LIST2CELL_X(value) value % p_state->field_width
#define MINES_GAME_LIST2CELL_Y(value) value / p_state->field_width

#define MINES_GAME_FIELD_CELL 9
#define MINES_GAME_FIELD_SIDEBAR_POSITION 120
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

#define MINES_PATTERN_BLANK 128
#define MINES_PATTERN_BOOM 129
#define MINES_PATTERN_CLEARED 130
#define MINES_PATTERN_FLAG 131
#define MINES_PATTERN_HOLD 132
#define MINES_PATTERN_MINE 133

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

typedef struct MinesBorderingCells {
    uint8_t cells[8];
    uint8_t count;
} MinesBorderingCells;

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

    bool cleared_rendered;
    bool controls_rendered;
    bool explosion_rendered;
    uint8_t manual_position;
    bool manual_rendered;
    uint8_t menu_position;
    bool menu_rendered;
    bool select_level_highlight;
    bool select_level_rendered;
    uint8_t sidebar_warning_hold;

    uint16_t timer_button;
    uint16_t timer_cursor;
    uint16_t timer_select_level_highlight;
    uint16_t timer_sidebar;

    void (*long_button_cb)(void *p);
    void (*short_button_cb)(void *p);
} MinesGameState;

static void mines_ui_draw_cell_pattern(uint8_t x, uint8_t y, uint8_t pattern_id)
{
    switch (pattern_id) {
    case 0:
        gfx_fill_rect(x - 1, y - 1, MINES_GAME_FIELD_CELL + 1,
                      MINES_GAME_FIELD_CELL + 1, 0x9492);
        break;

    case 1:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_1_bitmap, 0);
        break;

    case 2:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_2_bitmap, 0);
        break;

    case 3:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_3_bitmap, 0);
        break;

    case 4:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_4_bitmap, 0);
        break;

    case 5:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_5_bitmap, 0);
        break;

    case 6:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_6_bitmap, 0);
        break;

    case 7:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_7_bitmap, 0);
        break;

    case 8:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_8_bitmap, 0);
        break;

    case MINES_PATTERN_BLANK:
        display_draw_16bit_ext_bitmap(x - 1, y - 1, &mines_pattern_blank_bitmap,
                                      0);
        break;

    case MINES_PATTERN_BOOM:
        display_draw_16bit_ext_bitmap(x - 2, y - 2, &mines_pattern_boom_bitmap,
                                      0);
        break;

    case MINES_PATTERN_CLEARED:
        display_draw_16bit_ext_bitmap(x, y, &mines_pattern_cleared_bitmap, 0);
        break;

    case MINES_PATTERN_FLAG:
        display_draw_16bit_ext_bitmap(x, y, &mines_pattern_flag_bitmap, 0);

        break;
    case MINES_PATTERN_HOLD:
        display_draw_16bit_ext_bitmap(x, y, &mines_pattern_hold_bitmap, 0);
        break;

    case MINES_PATTERN_MINE:
        display_draw_16bit_ext_bitmap(x, y, &mines_pattern_mine_bitmap, 0);
        break;
    }
}

static void mines_ui_draw_cursor(uint8_t x, uint8_t y, uint8_t variant)
{
    uint16_t color = 0x9001;

    if (variant == 1) {
        color = 0xd6db;
    } else if (variant == 2) {
        color = 0x9492;
    }

    gfx_draw_rect(x, y, MINES_GAME_FIELD_CELL - 1, MINES_GAME_FIELD_CELL - 1,
                  color);
}

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

static void mines_calculate_bordering_cells(
    uint8_t x, uint8_t y, MinesBorderingCells *p_cells, MinesGameState *p_state,
    bool (*filter)(MinesGameState *p_state, uint8_t value))
{
    uint8_t bottom = y == p_state->field_height - 1 ? y : y + 1;
    uint8_t left = x == 0 ? 0 : x - 1;
    uint8_t right = x == p_state->field_width - 1 ? x : x + 1;
    uint8_t top = y == 0 ? 0 : y - 1;
    uint8_t value;

    for (uint8_t i = left; i <= right; i++) {
        for (uint8_t j = top; j <= bottom; j++) {
            value = MINES_GAME_CELL2LIST(i, j);

            if (i == x && j == y)
                continue;

            if (filter && !filter(p_state, value))
                continue;

            p_cells->cells[p_cells->count] = value;
            p_cells->count++;
        }
    }
}

static void mines_calculate_cell_origin(uint8_t x, uint8_t y, uint8_t width,
                                        uint8_t height, uint8_t *origin_x,
                                        uint8_t *origin_y)
{
    uint8_t offset_x =
        (MINES_GAME_FIELD_SIDEBAR_POSITION - width * MINES_GAME_FIELD_CELL) / 2;
    uint8_t offset_y = (80 - height * MINES_GAME_FIELD_CELL) / 2;

    *origin_x = offset_x + x * MINES_GAME_FIELD_CELL;
    *origin_y = offset_y + y * MINES_GAME_FIELD_CELL + 1;
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

static void mines_clear_cursor(MinesGameState *p_state)
{
    uint8_t origin_x, origin_y;

    if (p_state->cursor_activated) {
        mines_calculate_cell_origin(p_state->cursor_x, p_state->cursor_y,
                                    p_state->field_width, p_state->field_height,
                                    &origin_x, &origin_y);

        if (mines_game_is_opened(
                p_state,
                MINES_GAME_CELL2LIST(p_state->cursor_x, p_state->cursor_y))) {
            mines_ui_draw_cursor(origin_x, origin_y, 2);
        } else {
            mines_ui_draw_cursor(origin_x, origin_y, 1);
        }

        p_state->cursor_activated = false;
        MINES_TIMER_CANCEL(cursor);
    }
}

static void mines_game_action_flag(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    mines_clear_cursor(p_state);

    uint8_t origin_x, origin_y;
    mines_calculate_cell_origin(p_state->cursor_x, p_state->cursor_y,
                                p_state->field_width, p_state->field_height,
                                &origin_x, &origin_y);

    uint8_t value = MINES_GAME_CELL2LIST(p_state->cursor_x, p_state->cursor_y);

    if (mines_game_has_hold_at(p_state, value)) {
        mines_remove_from_list(p_state->holds, &p_state->holds_placed, value);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_BLANK);
    } else if (mines_game_has_flag_at(p_state, value)) {
        mines_remove_from_list(p_state->flags, &p_state->flags_placed, value);
        mines_add_to_list(p_state->holds, &p_state->holds_placed, value);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_HOLD);
    } else {
        mines_add_to_list(p_state->flags, &p_state->flags_placed, value);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_FLAG);
    }
}

static void mines_game_action_flags_warn(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    p_state->sidebar_warning_hold = 3;
    MINES_TIMER_CANCEL(sidebar);
}

static void mines_game_action_move_down(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    if (p_state->cursor_y < p_state->field_height - 1) {
        mines_clear_cursor(p_state);
        p_state->cursor_y++;
    }
}

static void mines_game_action_move_left(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    if (p_state->cursor_x > 0) {
        mines_clear_cursor(p_state);
        p_state->cursor_x--;
    }
}

static void mines_game_action_move_right(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    if (p_state->cursor_x < p_state->field_width - 1) {
        mines_clear_cursor(p_state);
        p_state->cursor_x++;
    }
}

static void mines_game_action_move_up(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    if (p_state->cursor_y > 0) {
        mines_clear_cursor(p_state);
        p_state->cursor_y--;
    }
}

static void mines_game_action_open(void *p)
{
    MinesGameState *p_state = (MinesGameState *)p;

    mines_clear_cursor(p_state);

    uint8_t origin_x, origin_y;
    uint8_t value = MINES_GAME_CELL2LIST(p_state->cursor_x, p_state->cursor_y);

    mines_calculate_cell_origin(p_state->cursor_x, p_state->cursor_y,
                                p_state->field_width, p_state->field_height,
                                &origin_x, &origin_y);

    if (mines_game_has_flag_at(p_state, value)) {
        mines_remove_from_list(p_state->flags, &p_state->flags_placed, value);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_BLANK);
    } else if (mines_game_has_hold_at(p_state, value)) {
        mines_remove_from_list(p_state->holds, &p_state->holds_placed, value);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_BLANK);
    } else if (mines_game_has_mine_at(p_state, value)) {
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_MINE);
        MINES_GAME_GOTO(MINES_GAME_STATE_EXPLOSION);
    } else {
        mines_add_to_list(p_state->opened, &p_state->opened_count, value);

        MinesBorderingCells mines_arround = {};

        mines_calculate_bordering_cells(p_state->cursor_x, p_state->cursor_y,
                                        &mines_arround, p_state,
                                        mines_game_has_mine_at);

        mines_ui_draw_cell_pattern(origin_x, origin_y, mines_arround.count);
    }

    gfx_update();
}

static void mines_game_state_game_task_buttons(MinesGameState *p_state)
{
    uint8_t value = MINES_GAME_CELL2LIST(p_state->cursor_x, p_state->cursor_y);

    switch (mines_buttons_read()) {
    case BUTTON_BACK:
        p_state->long_button_cb = NULL;
        p_state->short_button_cb = mines_game_action_move_left;

        if (!mines_game_is_opened(p_state, value)) {
            MINES_TIMER_START(button, 666);
            p_state->long_button_cb = mines_game_action_open;
        }

        break;

    case BUTTON_DOWN:
        p_state->long_button_cb = NULL;
        p_state->short_button_cb = mines_game_action_move_down;
        break;

    case BUTTON_ENTER:
        p_state->long_button_cb = NULL;
        p_state->short_button_cb = mines_game_action_move_right;

        if (mines_game_has_flag_at(p_state, value) ||
            mines_game_has_hold_at(p_state, value)) {
            MINES_TIMER_START(button, 666);
            p_state->long_button_cb = mines_game_action_flag;
        } else if (!mines_game_is_opened(p_state, value)) {
            uint8_t total_flagged =
                p_state->flags_placed + p_state->holds_placed;

            MINES_TIMER_START(button, 666);
            if (total_flagged < p_state->mines_total) {
                p_state->long_button_cb = mines_game_action_flag;
            } else {
                p_state->long_button_cb = mines_game_action_flags_warn;
            }
        }

        break;

    case BUTTON_UP:
        p_state->long_button_cb = NULL;
        p_state->short_button_cb = mines_game_action_move_up;
        break;

    case BUTTON_BACK_RELEASE:
    case BUTTON_DOWN_RELEASE:
    case BUTTON_ENTER_RELEASE:
    case BUTTON_UP_RELEASE:
        if (MINES_TIMER_ENDED(button) && p_state->long_button_cb) {
            p_state->long_button_cb(p_state);
        } else if (p_state->short_button_cb) {
            p_state->short_button_cb(p_state);
        }

        break;
    }
}

static void mines_game_state_game_task_cursor(MinesGameState *p_state)
{
    uint8_t origin_x, origin_y;

    if (MINES_TIMER_ENDED(cursor)) {
        if (p_state->cursor_activated) {
            mines_clear_cursor(p_state);
            MINES_TIMER_START(cursor, 500);
        } else {
            MINES_TIMER_START(cursor, 500);
            p_state->cursor_activated = true;

            mines_calculate_cell_origin(
                p_state->cursor_x, p_state->cursor_y, p_state->field_width,
                p_state->field_height, &origin_x, &origin_y);

            mines_ui_draw_cursor(origin_x, origin_y, 0);

            gfx_update();
        }
    }
}

static void mines_game_state_game_task_sidebar(MinesGameState *p_state)
{
    if (MINES_TIMER_ENDED(sidebar)) {
        if (p_state->sidebar_warning_hold > 0) {
            p_state->sidebar_warning_hold--;

            display_draw_16bit_ext_bitmap(MINES_GAME_FIELD_SIDEBAR_POSITION, 0,
                                          &mines_sidebar_warning_bitmap, 0);
        } else {
            char value[3];

            display_draw_16bit_ext_bitmap(MINES_GAME_FIELD_SIDEBAR_POSITION, 0,
                                          &mines_sidebar_bitmap, 0);

            gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);

            gfx_set_cursor(MINES_GAME_FIELD_SIDEBAR_POSITION + 7, 14);
            sprintf(value, "%d", p_state->mines_total);
            gfx_puts(value);

            gfx_set_cursor(MINES_GAME_FIELD_SIDEBAR_POSITION + 7, 37);
            sprintf(value, "%d", p_state->flags_placed);
            gfx_puts(value);

            gfx_set_cursor(MINES_GAME_FIELD_SIDEBAR_POSITION + 7, 61);
            sprintf(value, "%d", p_state->holds_placed);
            gfx_puts(value);

            gfx_update();
        }

        MINES_TIMER_START(sidebar, 1000);
    }
}

static void mines_game_state_game_task_win(MinesGameState *p_state)
{
    if (p_state->opened_count + p_state->mines_total >=
        p_state->field_width * p_state->field_height) {
        MINES_GAME_GOTO(MINES_GAME_STATE_CLEARED);
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

static void mines_game_state_cleared_handle(MinesGameState *p_state)
{
    if (!p_state->cleared_rendered) {
        uint8_t origin_x, origin_y, x, y;

        mines_clear_cursor(p_state);

        for (uint8_t i = 0; i < p_state->mines_total; i++) {
            x = MINES_GAME_LIST2CELL_X(p_state->mines[i]);
            y = MINES_GAME_LIST2CELL_Y(p_state->mines[i]);

            mines_calculate_cell_origin(x, y, p_state->field_width,
                                        p_state->field_height, &origin_x,
                                        &origin_y);

            mines_ui_draw_cell_pattern(origin_x, origin_y,
                                       MINES_PATTERN_CLEARED);
        }

        p_state->cleared_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->cleared_rendered = false;
        MINES_GAME_GOTO(MINES_GAME_STATE_CLEARED_MSG);
    }
}

static void mines_game_state_cleared_msg_handle(MinesGameState *p_state)
{
    if (!p_state->cleared_rendered) {
        display_draw_16bit_ext_bitmap(5, 12, &mines_message_cleared_bitmap, 0);
        p_state->cleared_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->cleared_rendered = false;
        MINES_GAME_GOTO(MINES_GAME_STATE_POST_MENU);
    }
}

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

static void mines_game_state_draw_canvas_handle(MinesGameState *p_state)
{
    uint8_t origin_x, origin_y;

    uint8_t height = p_state->field_height;
    uint8_t width = p_state->field_width;

    display_draw_16bit_ext_bitmap(0, 0, &mines_background_bitmap, 0);
    display_draw_16bit_ext_bitmap(MINES_GAME_FIELD_SIDEBAR_POSITION, 0,
                                  &mines_sidebar_bitmap, 0);

    mines_calculate_cell_origin(0, 0, width, height, &origin_x, &origin_y);

    gfx_fill_rect(origin_x - 4, origin_y - 4, width * MINES_GAME_FIELD_CELL + 7,
                  height * MINES_GAME_FIELD_CELL + 7, 0x0063);

    gfx_fill_rect(origin_x - 2, origin_y - 2, width * MINES_GAME_FIELD_CELL + 3,
                  height * MINES_GAME_FIELD_CELL + 3, 0x11cc);

    gfx_fill_rect(origin_x, origin_y, width * MINES_GAME_FIELD_CELL,
                  height * MINES_GAME_FIELD_CELL, DISPLAY_WHITE);

    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            mines_calculate_cell_origin(i, j, width, height, &origin_x,
                                        &origin_y);
            mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_BLANK);
        }
    }

    gfx_update();

    MINES_GAME_GOTO(MINES_GAME_STATE_GAME);
}

static void mines_game_state_explosion_handle(MinesGameState *p_state)
{
    if (!p_state->explosion_rendered) {
        uint8_t origin_x, origin_y, x, y;

        mines_calculate_cell_origin(p_state->cursor_x, p_state->cursor_y,
                                    p_state->field_width, p_state->field_height,
                                    &origin_x, &origin_y);

        nrf_delay_ms(500);
        mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_BOOM);
        nrf_delay_ms(500);

        for (uint8_t i = 0; i < p_state->mines_total; i++) {
            x = MINES_GAME_LIST2CELL_X(p_state->mines[i]);
            y = MINES_GAME_LIST2CELL_Y(p_state->mines[i]);

            if (x == p_state->cursor_x && y == p_state->cursor_y) {
                continue;
            }

            mines_calculate_cell_origin(x, y, p_state->field_width,
                                        p_state->field_height, &origin_x,
                                        &origin_y);

            mines_ui_draw_cell_pattern(origin_x, origin_y, MINES_PATTERN_MINE);
        }

        p_state->explosion_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->explosion_rendered = false;
        MINES_GAME_GOTO(MINES_GAME_STATE_EXPLOSION_MSG);
    }
}

static void mines_game_state_explosion_msg_handle(MinesGameState *p_state)
{
    if (!p_state->explosion_rendered) {
        display_draw_16bit_ext_bitmap(5, 12, &mines_message_boom_bitmap, 0);
        p_state->explosion_rendered = true;
    }

    if (mines_buttons_is_any_pushed()) {
        p_state->explosion_rendered = false;
        MINES_GAME_GOTO(MINES_GAME_STATE_MENU);
    }
}

static void mines_game_state_flag_handle(MinesGameState *p_state) {}

static void mines_game_state_game_handle(MinesGameState *p_state)
{
    mines_game_state_game_task_buttons(p_state);
    mines_game_state_game_task_cursor(p_state);
    mines_game_state_game_task_sidebar(p_state);
    mines_game_state_game_task_win(p_state);
}

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

    p_state->long_button_cb = NULL;
    p_state->short_button_cb = NULL;

    MINES_TIMER_CANCEL(button);
    MINES_TIMER_CANCEL(cursor);
    MINES_TIMER_CANCEL(sidebar);

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

    if (!MINES_TIMER_ENDED(select_level_highlight))
        MINES_TIMER_TICK(select_level_highlight);

    if (!MINES_TIMER_ENDED(sidebar))
        MINES_TIMER_TICK(sidebar);
}

void mines_application(void (*service_device)())
{
    MinesGameState state = {.cleared_rendered = false,
                            .controls_rendered = false,
                            .current_difficulty = 0,
                            .current_state = MINES_GAME_STATE_BOOT,
                            .cursor_activated = false,
                            .cursor_x = 0,
                            .cursor_y = 0,
                            .explosion_rendered = false,
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
