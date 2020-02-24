/*
 * Copyright 2018 Eric Tremblay <habscup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nordic_common.h>
#include <nrf.h>
#include <stdbool.h>
#include <string.h>

#include "drivers/buttons.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "menu.h"
#include "nsec_led_settings.h"
#include "nsec_led_settings_base.h"
#include "persistency.h"

#include "FreeRTOS.h"
#include "queue.h"

static menu_t g_menu;

static struct {
    const nsec_led_settings_base_element *elements;
    int num_elements;
    int current_value_index;
    led_settings_base_set_value_func set_value;
} g_cur_cfg;

static void show_actual(const char *label)
{
    char actual[50] = {0};

    snprintf(actual, 50, "Now: %s", label);

    gfx_fill_rect(LED_SET_VAL_POS, 6 * 15, 8, DISPLAY_WHITE);
    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void on_menu_select(uint8_t item)
{
    ASSERT(item < g_cur_cfg.num_elements);

    const nsec_led_settings_base_element *elem = &g_cur_cfg.elements[item];
    g_cur_cfg.set_value(elem->value);

    show_actual(elem->label);
}

static void draw_led_title(void)
{
    draw_title("LED CONFIG", 5, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

void led_settings_base_base_redraw(void)
{
    ASSERT(g_cur_cfg.current_value_index < g_cur_cfg.num_elements);

    const char *actual_value_label =
        g_cur_cfg.elements[g_cur_cfg.current_value_index].label;

    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    show_actual(actual_value_label);
    menu_ui_redraw_all(&g_menu);
}

bool led_settings_base_base_handle_button(button_t button)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(&g_menu, button);
    }

    return quit;
}

void nsec_led_settings_base_page_init(
    const nsec_led_settings_base_element *elements, int num_elements,
    int initial_value, led_settings_base_set_value_func set_value)
{
    static menu_item_s menu_items[15];

    ASSERT(num_elements <= ARRAY_SIZE(menu_items));

    for (int i = 0; i < num_elements; i++) {
        menu_items[i].label = elements[i].label;
        menu_items[i].handler = on_menu_select;
    }

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT, num_elements,
              menu_items, HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    int initial_index = 0;
    for (int i = 0; i < num_elements; i++) {
        if (initial_value == elements[i].value) {
            initial_index = i;
            break;
        }
    }

    menu_set_selected(&g_menu, initial_index);

    g_cur_cfg.elements = elements;
    g_cur_cfg.num_elements = num_elements;
    g_cur_cfg.set_value = set_value;
    g_cur_cfg.current_value_index = initial_index;
}
