//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "menu.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "string.h"
#include "drivers/controls.h"
#include "gfx_effect.h"

#define FONT_SIZE_WIDTH  (6)
#define FONT_SIZE_HEIGHT (8)

typedef struct {
    uint16_t pos_x;
    uint16_t pos_y;
    uint8_t col_width;
    uint8_t line_height;
    uint8_t item_count;
    uint8_t selected_item;
    uint8_t item_on_top;
    uint8_t is_handling_buttons;
    menu_item_s items[MENU_LIMIT_MAX_ITEM_COUNT];
} menu_state_t;

static menu_state_t menu;

static void menu_ui_redraw_items(uint8_t start, uint8_t end);
void menu_button_handler(button_t button);

void menu_init(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height, uint8_t item_count, menu_item_s * items) {
    menu.item_count = 0;
    menu.selected_item = 0;
    menu.item_on_top = 0;
    menu_set_position(pos_x, pos_y, width, height);
    gfx_fill_rect(pos_x, pos_y, width, height, DISPLAY_BLACK);
    for(uint8_t i = 0; i < item_count; i++) {
        menu_add_item(items + i);
    }
    menu.is_handling_buttons = 1;
    nsec_controls_add_handler(menu_button_handler);
}

void menu_close(void) {
    menu.is_handling_buttons = 0;
}

void menu_open(void) {
    menu.is_handling_buttons = 1;
}

void menu_set_position(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height) {
    menu.pos_x = pos_x;
    menu.pos_y = pos_y;
    menu.col_width = width / FONT_SIZE_WIDTH;
    menu.line_height = height / FONT_SIZE_HEIGHT;
}

void menu_add_item(menu_item_s * new_item) {
    if(menu.item_count >= MENU_LIMIT_MAX_ITEM_COUNT) {
        return;
    }
    else {
        memcpy(menu.items + menu.item_count++, new_item, sizeof(menu.items[0]));
    }
    menu_ui_redraw_items(menu.item_count - 1, menu.item_count - 1);
}

static void menu_ui_redraw_items(uint8_t start, uint8_t end) {
    if(start < menu.item_on_top) {
        start = menu.item_on_top;
    }
    else if(start > menu.item_on_top + menu.line_height) {
        return;
    }
    if(end > menu.item_on_top + menu.line_height) {
        end = menu.item_on_top + menu.line_height;
    }
    else if(end < start) {
        return;
    }
    gfx_fill_rect(menu.pos_x,
                 menu.pos_y + FONT_SIZE_HEIGHT * (start - menu.item_on_top),
                 menu.col_width * FONT_SIZE_WIDTH,
                 (end - start + 1) * FONT_SIZE_HEIGHT,
                 DISPLAY_BLACK);
    for(int item_index = start; item_index < menu.item_count && item_index <= end; item_index++) {
        gfx_set_cursor(menu.pos_x, menu.pos_y + (item_index - menu.item_on_top) * FONT_SIZE_HEIGHT);
        if(item_index == menu.selected_item) {
            gfx_set_text_background_color(DISPLAY_BLACK, DISPLAY_WHITE);
        }
        else {
            gfx_set_text_background_color(DISPLAY_WHITE,DISPLAY_BLACK);
        }
        const char * string = menu.items[item_index].label;
        if(strlen(string) <= menu.col_width) {
            gfx_puts(string);
        }
        else {
            char printable[menu.col_width + 1];
            strncpy(printable, string, sizeof(printable) - 2);
            printable[menu.col_width - 1] = '\x01'; // Elipse: ...
            printable[menu.col_width] = '\0';
            gfx_puts(printable);
        }
    }
    gfx_update();
}

void menu_ui_redraw_all(void) {
    menu_ui_redraw_items(menu.item_on_top, menu.item_on_top + menu.line_height);
}

void menu_change_selected_item(MENU_DIRECTION direction) {
    switch(direction) {
        case MENU_DIRECTION_DOWN: {
            if(menu.selected_item < menu.item_count) {
                menu.selected_item++;
                if(menu.selected_item >= menu.item_on_top + (menu.line_height - 1)) {
                    menu.item_on_top++;
                    menu_ui_redraw_all();
                }
                else {
                    menu_ui_redraw_items(menu.selected_item - 1, menu.selected_item);
                }
            }
            if (menu.selected_item == menu.item_count) {
                menu.selected_item = 0;
                menu.item_on_top = 0;
                menu_ui_redraw_all();
            }
        }
            break;
        case MENU_DIRECTION_UP: {
            if(menu.selected_item == 0) {
                menu.item_on_top = menu.item_count - 1;
                menu.selected_item = menu.item_count;
                menu_ui_redraw_all();
            }
            if(menu.selected_item > 0) {
                menu.selected_item--;
                if(menu.item_on_top > menu.selected_item) {
                    menu.item_on_top--;
                    menu_ui_redraw_all();
                }
                else {
                    menu_ui_redraw_items(menu.selected_item, menu.selected_item + 1);
                }
            }
        }
            break;
    }
}

void menu_trigger_action(void) {
    if(menu.items[menu.selected_item].handler != NULL) {
        menu.items[menu.selected_item].handler(menu.selected_item);
    }
}


void menu_button_handler(button_t button) {
    if(menu.is_handling_buttons) {
        switch (button) {
            case BUTTON_UP:
                menu_change_selected_item(MENU_DIRECTION_UP);
                break;
            case BUTTON_DOWN:
                menu_change_selected_item(MENU_DIRECTION_DOWN);
                break;
            case BUTTON_ENTER:
                menu_trigger_action();
                break;
            default:
                break;
        }
    }
}
