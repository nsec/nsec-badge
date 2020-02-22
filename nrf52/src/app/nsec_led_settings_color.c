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
#include "nsec_led_settings_color.h"
#include "persistency.h"

#include "FreeRTOS.h"
#include "queue.h"

#define RED_INDEX 0
#define GREEN_INDEX 1
#define BLUE_INDEX 2
#define WHITE_INDEX 3
#define BLACK_INDEX 4
#define YELLOW_INDEX 5
#define CYAN_INDEX 6
#define MAGENTA_INDEX 7
#define PURPLE_INDEX 8
#define ORANGE_INDEX 9

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static menu_t g_menu;
static uint8_t g_color_idx;

static void show_actual_color(uint8_t color_idx)
{
    int color = getArrayColor_WS2812FX(color_idx);

    char actual[50] = {0};
    if (color == RED) {
        snprintf(actual, 50, "Now: %s", "Red");
    } else if (color == GREEN) {
        snprintf(actual, 50, "Now: %s", "Green");
    } else if (color == BLUE) {
        snprintf(actual, 50, "Now: %s", "Blue");
    } else if (color == WHITE) {
        snprintf(actual, 50, "Now: %s", "White");
    } else if (color == BLACK) {
        snprintf(actual, 50, "Now: %s", "Black");
    } else if (color == YELLOW) {
        snprintf(actual, 50, "Now: %s", "Yellow");
    } else if (color == CYAN) {
        snprintf(actual, 50, "Now: %s", "Cyan");
    } else if (color == PURPLE) {
        snprintf(actual, 50, "Now: %s", "Purple");
    } else {
        snprintf(actual, 50, "Now: %s", "Orange");
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void set_color(uint8_t color_idx, int color)
{
    setArrayColor_packed_WS2812FX(color, color_idx);
    update_stored_color(0, color, color_idx, true);
    show_actual_color(color_idx);
}

static void save_color(uint8_t item)
{
    switch (item) {
    case RED_INDEX:
        set_color(g_color_idx, RED);
        break;
    case GREEN_INDEX:
        set_color(g_color_idx, GREEN);
        break;
    case BLUE_INDEX:
        set_color(g_color_idx, BLUE);
        break;
    case WHITE_INDEX:
        set_color(g_color_idx, WHITE);
        break;
    case BLACK_INDEX:
        set_color(g_color_idx, BLACK);
        break;
    case YELLOW_INDEX:
        set_color(g_color_idx, YELLOW);
        break;
    case CYAN_INDEX:
        set_color(g_color_idx, CYAN);
        break;
    case MAGENTA_INDEX:
        set_color(g_color_idx, MAGENTA);
        break;
    case PURPLE_INDEX:
        set_color(g_color_idx, PURPLE);
        break;
    case ORANGE_INDEX:
        set_color(g_color_idx, ORANGE);
        break;
    default:
        break;
    }
}

static const menu_item_s color_items[] = {
    {
        .label = "Red",
        .handler = save_color,
    },
    {
        .label = "Green",
        .handler = save_color,
    },
    {
        .label = "Blue",
        .handler = save_color,
    },
    {
        .label = "White",
        .handler = save_color,
    },
    {
        .label = "Black",
        .handler = save_color,
    },
    {
        .label = "Yellow",
        .handler = save_color,
    },
    {
        .label = "Cyan",
        .handler = save_color,
    },
    {
        .label = "Magenta",
        .handler = save_color,
    },
    {
        .label = "Purple",
        .handler = save_color,
    },
    {
        .label = "Orange",
        .handler = save_color,
    },
};

static void draw_led_title(void)
{
    draw_title("LED CONFIG", 5, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

static void redraw_led_settings_color(uint8_t color_idx, menu_t *menu)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    show_actual_color(color_idx);
    menu_ui_redraw_all(menu);
}

static bool led_setting_color_handle_buttons(button_t button, menu_t *menu)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(menu, button);
    }

    return quit;
}

void nsec_show_led_settings_color(uint8_t color_idx)
{
    int initial_index = 0;

    g_color_idx = color_idx;

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(color_items), color_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);

    switch (getArrayColor_WS2812FX(color_idx)) {
    case RED:
        initial_index = RED_INDEX;
        break;
    case GREEN:
        initial_index = GREEN_INDEX;
        break;
    case BLUE:
        initial_index = BLUE_INDEX;
        break;
    case WHITE:
        initial_index = WHITE_INDEX;
        break;
    case BLACK:
        initial_index = BLACK_INDEX;
        break;
    case YELLOW:
        initial_index = YELLOW_INDEX;
        break;
    case CYAN:
        initial_index = CYAN_INDEX;
        break;
    case MAGENTA:
        initial_index = MAGENTA_INDEX;
        break;
    case PURPLE:
        initial_index = PURPLE_INDEX;
        break;
    case ORANGE:
        initial_index = ORANGE_INDEX;
        break;
    default:
        break;
    }

    menu_set_selected(&g_menu, initial_index);

    redraw_led_settings_color(color_idx, &g_menu);

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, portMAX_DELAY);
        APP_ERROR_CHECK_BOOL(ret == pdTRUE);

        bool quit = led_setting_color_handle_buttons(btn, &g_menu);

        if (quit) {
            break;
        }
    }
}
