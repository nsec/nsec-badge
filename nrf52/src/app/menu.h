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

void menu_handler_init(void);
void menu_init(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height,
               uint8_t item_count, menu_item_s *items, uint16_t text_color,
               uint16_t bg_color);
void menu_set_position(uint16_t pos_x, uint16_t pos_y, uint16_t width,
                       uint16_t height);
void menu_add_item(menu_item_s *new_item);
void menu_ui_redraw_all(void);
void menu_change_selected_item(MENU_DIRECTION direction);
void menu_trigger_action(void);
void menu_close(void);
void menu_open(void);
void menu_button_handler(button_t button);

#endif
