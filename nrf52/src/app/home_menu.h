//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef home_menu_h
#define home_menu_h

#include <nrf.h>
#include <stdio.h>
#include <stdbool.h>

enum home_state {
    HOME_STATE_CLOSED,
    HOME_STATE_MENU,
    HOME_STATE_MENU_SELECTED,
    HOME_STATE_SETTINGS,
    HOME_STATE_SETTINGS_SELECTED
};

struct title {
    char text[32];
    uint8_t pos_x;
    uint8_t pos_y;
    uint16_t text_color;
    uint16_t bg_color;
};

void show_home_menu(enum home_state state);
void draw_home_menu_bar(void);
void redraw_home_menu_burger_selected(void);
void draw_title(struct title *title);
void draw_settings_title(void);
void draw_main_menu_title(void);
void home_menu_application(void);
bool is_at_home_menu(void);

#endif /* home_menu_h */
