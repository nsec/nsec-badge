//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef menu_h
#define menu_h

#include "drivers/controls.h"
#include <stdint.h>

#define MENU_LIMIT_MAX_ITEM_COUNT (60)

typedef struct {
    const char *label;
    void (*handler)(uint8_t item_index);
} menu_item_s;

typedef enum {
    MENU_DIRECTION_UP,
    MENU_DIRECTION_DOWN,
} MENU_DIRECTION;

typedef struct {
    uint16_t pos_x;
    uint16_t pos_y;
    uint8_t col_width;

    // How many items fit fully in the menu height.
    uint8_t item_count_per_page;
    uint8_t item_count;

    // Selected item, 0-based.
    uint8_t selected_item;

    uint8_t item_on_top;
    uint16_t text_color;
    uint16_t bg_color;
    const menu_item_s *items;
} menu_t;

void menu_init(menu_t *menu, uint16_t pos_x, uint16_t pos_y, uint16_t width,
               uint16_t height, uint8_t item_count, const menu_item_s *items,
               uint16_t text_color, uint16_t bg_color);
void menu_ui_redraw_all(menu_t *menu);
void menu_button_handler(menu_t *menu, button_t button);

#endif
