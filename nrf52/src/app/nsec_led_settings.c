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

#include <nrf.h>
#include <nordic_common.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ble/nsec_ble.h"
#include "drivers/buttons.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "menu.h"
#include "nsec_led_settings.h"
#include "nsec_led_settings_brightness.h"
#include "nsec_led_settings_color.h"
#include "nsec_led_settings_speed.h"
#include "nsec_settings.h"
#include "persistency.h"
#include "status_bar.h"

#include "FreeRTOS.h"
#include "queue.h"

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static void show_brightness_menu(uint8_t item);
static void show_speed_menu(uint8_t item);
static void show_color1_menu(uint8_t item);
static void show_color2_menu(uint8_t item);
static void show_color3_menu(uint8_t item);
static void save_reverse(uint8_t item);
static void save_control(uint8_t item);
static void show_reverse_menu(uint8_t item);
static void show_control_menu(uint8_t item);

static const menu_item_s settings_items[] = {
    {
        .label = "Led brightness",
        .handler = show_brightness_menu,
    },
    {
        .label = "Led speed",
        .handler = show_speed_menu,
    },
    {
        .label = "Led first color",
        .handler = show_color1_menu,
    },
    {
        .label = "Led second color",
        .handler = show_color2_menu,
    },
    {
        .label = "Led third color",
        .handler = show_color3_menu,
    },
    {
        .label = "Reverse pattern",
        .handler = show_reverse_menu,
    },
    {
        .label = "Turn led on/off",
        .handler = show_control_menu,
    },
};

static const menu_item_s reverse_items[] = {
    {
        .label = "False",
        .handler = save_reverse,
    },
    {
        .label = "True",
        .handler = save_reverse,
    },
};

static const menu_item_s control_items[] = {
    {
        .label = "OFF",
        .handler = save_control,
    },
    {
        .label = "ON",
        .handler = save_control,
    },
};

static void draw_led_title(void)
{
    draw_title("LED CONFIG", 5, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

static menu_t g_menu;

static void redraw_led_settings(menu_t *menu)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    menu_ui_redraw_all(menu);
}

static void show_brightness_menu(uint8_t item) {
    nsec_show_led_settings_brightness();
    redraw_led_settings(&g_menu);
}

static void show_speed_menu(uint8_t item) {
    nsec_show_led_settings_speed();
    redraw_led_settings(&g_menu);
}

static void show_color_menu(uint8_t color_idx)
{
    nsec_show_led_settings_color(color_idx);
    redraw_led_settings(&g_menu);
}

static void show_color1_menu(uint8_t item)
{
    return show_color_menu(0);
}

static void show_color2_menu(uint8_t item)
{
    return show_color_menu(1);
}

static void show_color3_menu(uint8_t item)
{
    return show_color_menu(2);
}

static void show_actual_reverse(void)
{
    bool reverse = getReverse_WS2812FX();
    char actual[10] = {0};
    if (reverse) {
        snprintf(actual, 50, "Now: %s", "True");
    } else {
        snprintf(actual, 50, "Now: %s", "False");
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void show_reverse_menu(uint8_t item) {
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    show_actual_reverse();

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(reverse_items), reverse_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static void save_reverse(uint8_t item) {
    setReverse_WS2812FX((bool)item);
    update_stored_reverse(0, (bool)item, true);
    show_reverse_menu(0);
}

static void show_actual_control(void)
{
    bool control = isRunning_WS2812FX();
    char actual[10] = {0};
    if (control) {
        snprintf(actual, 50, "Now: %s", "ON");
    } else {
        snprintf(actual, 50, "Now: %s", "OFF");
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void show_control_menu(uint8_t item) {
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    show_actual_control();

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(control_items), control_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static void save_control(uint8_t item) {
    if (item) {
        start_WS2812FX();
    } else {
        stop_WS2812FX();
    }
    update_stored_control((bool)item, true);
    show_control_menu(0);
}

static bool led_setting_handle_buttons(button_t button, menu_t *menu)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(menu, button);
    }

    return quit;
}

void nsec_show_led_settings(void)
{
    menu_init(&g_menu, GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(settings_items), settings_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);

    redraw_led_settings(&g_menu);

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, portMAX_DELAY);
        APP_ERROR_CHECK_BOOL(ret == pdTRUE);

        bool quit = led_setting_handle_buttons(btn, &g_menu);

        if (quit) {
            break;
        }
    }
}
