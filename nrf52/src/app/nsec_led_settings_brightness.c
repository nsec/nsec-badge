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
#include <stdio.h>
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
#include "nsec_settings.h"
#include "persistency.h"
#include "status_bar.h"

#include "FreeRTOS.h"
#include "queue.h"

#define BRIGHNESS_MENU_INDEX 0

#define SUPER_LOW_BRIGHTNESS_INDEX 0
#define LOW_BRIGHTNESS_INDEX 1
#define MEDIUM_BRIGHTNESS_INDEX 2
#define HIGH_BRIGHTNESS_INDEX 3
#define MAX_BRIGHTNESS_INDEX 4

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static void show_brightness_menu(uint8_t item);
static void show_speed_menu(uint8_t item);
static void show_color_menu(uint8_t item);
static void save_brightness(uint8_t item);
static void save_speed(uint8_t item);
static void save_color(uint8_t item);
static void save_reverse(uint8_t item);
static void save_control(uint8_t item);
static void show_reverse_menu(uint8_t item);
static void show_control_menu(uint8_t item);
static void set_led_default(uint8_t item);

static const menu_item_s brightness_items[] = {
    {
        .label = "Super low",
        .handler = save_brightness,
    },
    {
        .label = "Low",
        .handler = save_brightness,
    },
    {
        .label = "Medium",
        .handler = save_brightness,
    },
    {
        .label = "High",
        .handler = save_brightness,
    },
    {
        .label = "Max",
        .handler = save_brightness,
    },
};

static void draw_led_title(void)
{
    draw_title("LED CONFIG", 5, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

static menu_t g_menu;

static void redraw_led_settings_brightness(menu_t *menu)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    menu_ui_redraw_all(menu);
}

static void show_actual_brightness(void)
{
    uint8_t brightness = getBrightness_WS2812FX();
    char actual[50] = {0};
    if (brightness <= SUPER_LOW_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Super Low");
    } else if (brightness <= LOW_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Low");
    } else if (brightness <= MEDIUM_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "Medium");
    } else if (brightness <= HIGH_BRIGHTNESS) {
        snprintf(actual, 50, "Now: %s", "High");
    } else {
        snprintf(actual, 50, "Now: %s", "Max");
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void show_brightness_menu(uint8_t item)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    show_actual_brightness();

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(brightness_items), brightness_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);
}

static void save_brightness(uint8_t item)
{
    switch (item) {
    case SUPER_LOW_BRIGHTNESS_INDEX:
        setBrightness_WS2812FX(SUPER_LOW_BRIGHTNESS);
        break;
    case LOW_BRIGHTNESS_INDEX:
        setBrightness_WS2812FX(LOW_BRIGHTNESS);
        break;
    case MEDIUM_BRIGHTNESS_INDEX:
        setBrightness_WS2812FX(MEDIUM_BRIGHTNESS);
        break;
    case HIGH_BRIGHTNESS_INDEX:
        setBrightness_WS2812FX(HIGH_BRIGHTNESS);
        break;
    case MAX_BRIGHTNESS_INDEX:
        setBrightness_WS2812FX(MAX_BRIGHTNESS);
        break;
    default:
        break;
    }

    update_stored_brightness(getBrightness_WS2812FX(), true);
    show_brightness_menu(0);
}

static bool led_setting_brightness_handle_buttons(button_t button, menu_t *menu)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(menu, button);
    }

    return quit;
}

void nsec_show_led_settings_brightness(void)
{
    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(brightness_items), brightness_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    redraw_led_settings_brightness(&g_menu);

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, portMAX_DELAY);
        APP_ERROR_CHECK_BOOL(ret == pdTRUE);

        bool quit = led_setting_brightness_handle_buttons(btn, &g_menu);

        if (quit) {
            break;
        }
    }
}
