//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "app_error.h"

#include "menu.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "string.h"
#include "utils.h"

#define FONT_SIZE_WIDTH (6)
#define FONT_SIZE_HEIGHT (8)

static void menu_ui_redraw_items(menu_t *menu, uint8_t start, uint8_t end);

void menu_init(menu_t *menu, uint16_t pos_x, uint16_t pos_y, uint16_t width,
               uint16_t height, uint8_t item_count, menu_item_s *items,
               uint16_t text_color, uint16_t bg_color)
{
    menu->item_count = 0;
    menu->selected_item = 0;
    menu->item_on_top = 0;
    menu->text_color = text_color;
    menu->bg_color = bg_color;
    menu_set_position(menu, pos_x, pos_y, width, height);
    gfx_fill_rect(pos_x, pos_y, width, height, bg_color);
    for (uint8_t i = 0; i < item_count; i++) {
        menu_add_item(menu, items + i);
    }
    menu->is_handling_buttons = 1;
}

void menu_close(menu_t *menu)
{
    menu->is_handling_buttons = 0;
}

void menu_open(menu_t *menu)
{
    menu->is_handling_buttons = 1;
}

void menu_set_position(menu_t *menu, uint16_t pos_x, uint16_t pos_y,
                       uint16_t width, uint16_t height)
{
    menu->pos_x = pos_x;
    menu->pos_y = pos_y;
    menu->col_width = width / FONT_SIZE_WIDTH;
    menu->item_count_per_page = height / FONT_SIZE_HEIGHT;
}

void menu_add_item(menu_t *menu, menu_item_s *new_item)
{
    if (menu->item_count >= MENU_LIMIT_MAX_ITEM_COUNT) {
        return;
    } else {
        memcpy(menu->items + menu->item_count++, new_item,
               sizeof(menu->items[0]));
    }
    menu_ui_redraw_items(menu, menu->item_count - 1, menu->item_count - 1);
}

static void menu_ui_redraw_items(menu_t *menu, uint8_t start, uint8_t end)
{
    APP_ERROR_CHECK_BOOL(start <= end);

    /* Last item displayed. */
    uint8_t item_at_bottom = menu->item_on_top + menu->item_count_per_page - 1;

    /* Is the region of the menu to redraw completely outside what is currently displayed? */
    if (start > item_at_bottom || end < menu->item_on_top) {
        return;
    }

    /* Don't redraw items outside what is displayed. */
    start = max(start, menu->item_on_top);
    end = min(end, item_at_bottom);

    gfx_set_text_size(1);
    gfx_fill_rect(menu->pos_x,
                  menu->pos_y + FONT_SIZE_HEIGHT * (start - menu->item_on_top),
                  menu->col_width * FONT_SIZE_WIDTH,
                  (end - start + 1) * FONT_SIZE_HEIGHT, menu->bg_color);

    for (int item_index = start;
         item_index < menu->item_count && item_index <= end; item_index++) {
        gfx_set_cursor(menu->pos_x,
                       menu->pos_y +
                           (item_index - menu->item_on_top) * FONT_SIZE_HEIGHT);
        if (item_index == menu->selected_item) {
            gfx_set_text_background_color(menu->bg_color, menu->text_color);
        } else {
            gfx_set_text_background_color(menu->text_color, menu->bg_color);
        }
        const char *string = menu->items[item_index].label;
        if (strlen(string) <= menu->col_width) {
            gfx_puts(string);
        } else {
            char printable[menu->col_width + 1];
            strncpy(printable, string, sizeof(printable) - 2);
            printable[menu->col_width - 1] = '\x01'; // Elipse: ...
            printable[menu->col_width] = '\0';
            gfx_puts(printable);
        }
    }
    gfx_update();
}

void menu_ui_redraw_all(menu_t *menu)
{
    menu_ui_redraw_items(menu, menu->item_on_top,
                         menu->item_on_top + menu->item_count_per_page);
}

/* Compute which item should be on top, given the current selection. */
static uint8_t compute_item_on_top(menu_t *m)
{
    return m->selected_item - (m->selected_item % m->item_count_per_page);
}

void menu_change_selected_item(menu_t *menu, MENU_DIRECTION direction)
{
    switch (direction) {
    case MENU_DIRECTION_DOWN: {
        if (menu->selected_item < (menu->item_count - 1)) {
            // Pressed down while not on the last item.
            menu->selected_item++;

            uint8_t item_on_top = compute_item_on_top(menu);
            if (item_on_top != menu->item_on_top) {
                menu->item_on_top = item_on_top;
                menu_ui_redraw_all(menu);
            } else {
                menu_ui_redraw_items(menu, menu->selected_item - 1,
                                     menu->selected_item);
            }
        } else {
            // Pressed down while on the last item.
            menu->selected_item = 0;

            uint8_t item_on_top = compute_item_on_top(menu);
            if (item_on_top != menu->item_on_top) {
                menu->item_on_top = item_on_top;
                menu_ui_redraw_all(menu);
            } else {
                menu_ui_redraw_items(menu, 0, 0);
                menu_ui_redraw_items(menu, menu->item_count - 1,
                                     menu->item_count - 1);
            }
        }
    } break;
    case MENU_DIRECTION_UP: {
        if (menu->selected_item == 0) {
            // Pressed up while at the first item.
            menu->selected_item = menu->item_count - 1;

            uint8_t item_on_top = compute_item_on_top(menu);
            if (item_on_top != menu->item_on_top) {
                menu->item_on_top = item_on_top;
                menu_ui_redraw_all(menu);
            } else {
                menu_ui_redraw_items(menu, 0, 0);
                menu_ui_redraw_items(menu, menu->item_count - 1,
                                     menu->item_count - 1);
            }
        } else {
            // Pressed up while not at the first item.
            menu->selected_item--;

            uint8_t item_on_top = compute_item_on_top(menu);
            if (item_on_top != menu->item_on_top) {
                menu->item_on_top = item_on_top;
                menu_ui_redraw_all(menu);
            } else {
                menu_ui_redraw_items(menu, menu->selected_item,
                                     menu->selected_item + 1);
            }
        }
    } break;
    }
}

void menu_trigger_action(menu_t *menu)
{
    if (menu->items[menu->selected_item].handler != NULL) {
        menu->items[menu->selected_item].handler(menu->selected_item);
    }
}

void menu_button_handler(menu_t *menu, button_t button)
{
    if (menu->is_handling_buttons) {
        switch (button) {
        case BUTTON_UP:
            menu_change_selected_item(menu, MENU_DIRECTION_UP);
            break;
        case BUTTON_DOWN:
            menu_change_selected_item(menu, MENU_DIRECTION_DOWN);
            break;
        case BUTTON_ENTER:
            menu_trigger_action(menu);
            break;
        default:
            break;
        }
    }
}
