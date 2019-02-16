//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "home_menu.h"
#include "drivers/display.h"
#include "drivers/controls.h"
#include "gfx_effect.h"
#include "gui.h"
#include <string.h>

#include "images/settings_off_bitmap.c"
#include "images/settings_on_bitmap.c"

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static struct bitmap settings_off, settings_on;

static void home_menu_handle_buttons(button_t button);

enum home_state {
    HOME_STATE_CLOSED,
    HOME_STATE_MENU,
    HOME_STATE_MENU_SELECTED,
    HOME_STATE_SETTINGS,
    HOME_STATE_SETTINGS_SELECTED
};

static enum home_state _state = HOME_STATE_CLOSED;

static void draw_burger_menu_icon(int16_t x, int16_t y, uint16_t color)
{
    gfx_fill_rect(x, y, 3, 3, color);
    gfx_fill_rect(x, y+6, 3, 3, color);
    gfx_fill_rect(x, y+12, 3, 3, color);

    gfx_fill_rect(x+6, y, 12, 3, color);
    gfx_fill_rect(x+6, y+6, 12, 3, color);
    gfx_fill_rect(x+6, y+12, 12, 3, color);
}

static void draw_cursor(void)
{
    if (_state == HOME_STATE_MENU) {
        gfx_fill_triangle(HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y,
            HOME_MENU_POS_X + 7, SETTINGS_MENU_CUR_Y + 5,
            HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y + 10,
            HOME_MENU_BG_COLOR);

        gfx_fill_triangle(HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y,
            HOME_MENU_POS_X + 7, BURGER_MENU_CUR_Y + 5,
            HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y + 10,
            DISPLAY_RED);
    } else {
        gfx_fill_triangle(HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y,
            HOME_MENU_POS_X + 7, BURGER_MENU_CUR_Y + 5,
            HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y + 10,
            HOME_MENU_BG_COLOR);

        gfx_fill_triangle(HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y,
            HOME_MENU_POS_X + 7, SETTINGS_MENU_CUR_Y + 5,
            HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y + 5,
            DISPLAY_RED);
    }
}

static void draw_home_menu(void)
{
    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT,
        HOME_MENU_BG_COLOR);

    draw_burger_menu_icon(BURGER_MENU_POS, DISPLAY_WHITE);
    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_off);

    draw_cursor();

    // TODO Show logo screen instead
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height,
        DISPLAY_BLACK);
}

void show_home_menu(void)
{
    settings_off = (struct bitmap){
        .image = settings_off_bitmap,
        .width = settings_off_bitmap_width,
        .height = settings_off_bitmap_height,
        .bg_color = HOME_MENU_BG_COLOR
    };

    settings_on = (struct bitmap){
        .image = settings_on_bitmap,
        .width = settings_on_bitmap_width,
        .height = settings_on_bitmap_height,
        .bg_color = DISPLAY_WHITE
    };

    _state = HOME_STATE_MENU;

    nsec_controls_add_handler(home_menu_handle_buttons);

    draw_home_menu();
}

static void open_burger_menu(void)
{
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height,
        DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2,
        DISPLAY_WHITE);

    draw_burger_menu_icon(BURGER_MENU_POS, HOME_MENU_BG_COLOR);

    gfx_set_cursor(BURGER_MENU_TITLE_X, 5);
    gfx_set_text_size(2);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts("MENU");
}

static void open_settings_menu(void)
{
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height,
        DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS_X, HOME_MENU_POS_Y + (HOME_MENU_HEIGHT / 2),
        HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2, DISPLAY_WHITE);

    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_on);

    gfx_set_cursor(SETTINGS_MENU_TITLE_X, 5);
    gfx_set_text_size(2);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts("SETTINGS");
}

static void home_menu_handle_buttons(button_t button)
{
    switch (button) {
        case BUTTON_BACK:
            if (_state == HOME_STATE_MENU || _state == HOME_STATE_SETTINGS) {
                // Close menu (show intro ???); or maybe nothing
            } else if (_state == HOME_STATE_MENU_SELECTED) {
                _state = HOME_STATE_MENU;
                draw_home_menu();
            } else {
                _state = HOME_STATE_SETTINGS;
                draw_home_menu();
            }
        break;

        case BUTTON_DOWN:
            if (_state == HOME_STATE_MENU) {
                _state = HOME_STATE_SETTINGS;
                draw_cursor();
            } else if (_state == HOME_STATE_SETTINGS) {
                _state = HOME_STATE_MENU;
                draw_cursor();
            }
        break;

        case BUTTON_UP:
            if (_state == HOME_STATE_MENU) {
                _state = HOME_STATE_SETTINGS;
                draw_cursor();
            } else if (_state == HOME_STATE_SETTINGS) {
                _state = HOME_STATE_MENU;
                draw_cursor();
            }

        break;

        case BUTTON_ENTER:
            if (_state == HOME_STATE_MENU) {
                _state = HOME_STATE_MENU_SELECTED;
                open_burger_menu();
            } else {
                _state = HOME_STATE_SETTINGS_SELECTED;
                open_settings_menu();
            }
        break;
    }
}