//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef home_menu_h
#define home_menu_h

enum home_state {
    HOME_STATE_CLOSED,
    HOME_STATE_MENU,
    HOME_STATE_MENU_SELECTED,
    HOME_STATE_SETTINGS,
    HOME_STATE_SETTINGS_SELECTED
};

void show_home_menu(enum home_state state);
void draw_home_menu_bar(void);
void redraw_home_menu_burger_selected(void);
void draw_title(void);

#endif /* home_menu_h */
