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

#include "ui_page.h"

void redraw_home_menu_burger_selected(void);
void draw_title(const char *text, uint8_t pos_x, uint8_t pos_y,
                uint16_t text_color, uint16_t bg_color);
void draw_settings_title(void);
void draw_main_menu_title(void);

extern const ui_page home_page;

bool is_at_home_menu(void);

#endif /* home_menu_h */
