//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <stdint.h>

#define MENU_LIMIT_MAX_ITEM_COUNT (10)

typedef struct {
	char * label;
	void (*handler)(uint8_t item_index);
} menu_item_s;

typedef enum {
	MENU_DIRECTION_UP,
	MENU_DIRECTION_DOWN,
	MENU_DIRECTION_LEFT,
	MENU_DIRECTION_RIGHT,
} MENU_DIRECTION;

void menu_init(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height, uint8_t item_count, menu_item_s * items);
void menu_set_position(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height);
void menu_add_item(menu_item_s * new_item);
void menu_ui_redraw_all(void);
void menu_change_selected_item(MENU_DIRECTION direction);
void menu_trigger_action(void);
void menu_close(void);
