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

#include "images/settings_bitmap.c"

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static struct bitmap settings;

static void home_menu_handle_buttons(button_t button);

enum home_state {
    HOME_STATE_CLOSED,
    HOME_STATE_MENU
};

static enum home_state _state = HOME_STATE_CLOSED;

void draw_burger_menu_icon(int16_t x, int16_t y, uint16_t color)
{
    gfx_fill_rect(x, y, 3, 3, color);
    gfx_fill_rect(x, y+6, 3, 3, color);
    gfx_fill_rect(x, y+12, 3, 3, color);

    gfx_fill_rect(x+6, y, 12, 3, color);
    gfx_fill_rect(x+6, y+6, 12, 3, color);
    gfx_fill_rect(x+6, y+12, 12, 3, color);
}

void show_home_menu(void)
{
    settings = (struct bitmap){
        .image = settings_bitmap,
        .width = settings_bitmap_width,
        .height = settings_bitmap_height,
        .bg_color = HOME_MENU_BG_COLOR
    };

    _state = HOME_STATE_MENU;

    nsec_controls_add_handler(home_menu_handle_buttons);

    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT,
        HOME_MENU_BG_COLOR);

    draw_burger_menu_icon(BURGER_MENU_POS, DISPLAY_WHITE);
    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings);
}

static void home_menu_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
    }
}