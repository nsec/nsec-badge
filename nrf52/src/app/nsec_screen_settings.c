/*
 * Copyright 2019 Michael Jeanson <mjeanson@gmail.com>
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
#include "drivers/controls.h"
#include "app/application.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "menu.h"
#include "nsec_led_settings.h"
#include "nsec_settings.h"
#include "persistency.h"
#include "status_bar.h"

enum screen_setting_state {
    SCREEN_SETTING_STATE_CLOSED,
    SCREEN_SETTING_STATE_MENU,
    SCREEN_SETTING_STATE_BRIGHTNESS,
    SCREEN_SETTING_STATE_FIX,
};

enum screen_brightness_levels {
    SCREEN_BRIGHTNESS_MIN = 1,
    SCREEN_BRIGHTNESS_LOW = 25,
    SCREEN_BRIGHTNESS_MED = 50,
    SCREEN_BRIGHTNESS_HIGH = 75,
    SCREEN_BRIGHTNESS_MAX = 100,
};

static enum screen_setting_state _state = SCREEN_SETTING_STATE_CLOSED;

static void show_screen_brightness_menu(uint8_t item);
static void show_screen_fix_menu(uint8_t item);
static void save_screen_brightness(uint8_t item);
static void save_screen_fix(uint8_t item);
static void screen_setting_handle_buttons(button_t button);

static menu_item_s screen_settings_items[] = {
    {
        .label = "Screen brightness",
        .handler = show_screen_brightness_menu,
    },
    {
        .label = "Screen fix",
        .handler = show_screen_fix_menu,
    }};

static menu_item_s screen_brightness_items[] = {
    {
        .label = "Min",
        .handler = save_screen_brightness,
    },
    {
        .label = "Low",
        .handler = save_screen_brightness,
    },
    {
        .label = "Medium",
        .handler = save_screen_brightness,
    },
    {
        .label = "High",
        .handler = save_screen_brightness,
    },
    {
        .label = "Max",
        .handler = save_screen_brightness,
    }};

static menu_item_s screen_fix_items[] = {
    {
        .label = "Green pill",
        .handler = save_screen_fix,
    },
    {
        .label = "Red pill",
        .handler = save_screen_fix,
    }};

static void draw_screen_title(void)
{
    struct title title;

    title.pos_y = 5;
    title.pos_x = 5;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;

    strcpy(title.text, "SCREEN");
    draw_title(&title);
}

/* Menu entry point */
void nsec_show_screen_settings(void)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);

    draw_screen_title();

    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(screen_settings_items), screen_settings_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    nsec_controls_add_handler(screen_setting_handle_buttons);
    _state = SCREEN_SETTING_STATE_MENU;
}

void show_actual_screen_brightness(void)
{
    uint16_t brightness = get_stored_display_brightness();
    char actual[50] = {0};

    switch (brightness) {
    case SCREEN_BRIGHTNESS_MIN:
        snprintf(actual, 50, "Now: %s", "Minimum");
        break;
    case SCREEN_BRIGHTNESS_LOW:
        snprintf(actual, 50, "Now: %s", "Low");
        break;
    case SCREEN_BRIGHTNESS_MED:
        snprintf(actual, 50, "Now: %s", "Medium");
        break;
    case SCREEN_BRIGHTNESS_HIGH:
        snprintf(actual, 50, "Now: %s", "High");
        break;
    case SCREEN_BRIGHTNESS_MAX:
    default:
        snprintf(actual, 50, "Now: %s", "Max");
        break;
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

/* Screen brightness sub-menu */
static void show_screen_brightness_menu(uint8_t item)
{
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    show_actual_screen_brightness();

    menu_init(LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(screen_brightness_items), screen_brightness_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    _state = SCREEN_SETTING_STATE_BRIGHTNESS;
}

/* Apply and persist screen brightness setting */
static void save_screen_brightness(uint8_t item)
{
    uint8_t b = (item == 0) ? 1 : item * 25;
    display_set_brightness(b);
    update_stored_display_brightness(b);
}

void show_actual_screen_fix(void)
{
    uint8_t model = get_stored_display_model();
    char actual[50] = {0};

    switch (model) {
    case DISPLAY_MODEL_GREEN:
        snprintf(actual, 50, "Now: %s", "Green pill");
        break;
    case DISPLAY_MODEL_RED:
        snprintf(actual, 50, "Now: %s", "Red pill");
        break;
    }

    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

/* Screen fix sub-menu */
static void show_screen_fix_menu(uint8_t item)
{
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    show_actual_screen_fix();

    menu_init(LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(screen_fix_items), screen_fix_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    _state = SCREEN_SETTING_STATE_FIX;
}

static void dummy_app(void (*service_callback)()) {
    application_clear();
}

/* Apply and persist screen fix setting */
static void save_screen_fix(uint8_t item)
{
    display_set_model(item);
    update_stored_display_model(item);
    menu_close();
    application_set(dummy_app);
}

static void screen_setting_handle_buttons(button_t button)
{
    if (button == BUTTON_BACK) {
        switch (_state) {
        case SCREEN_SETTING_STATE_MENU:
            _state = SCREEN_SETTING_STATE_CLOSED;
            menu_close();
            nsec_setting_show();
            break;

        case SCREEN_SETTING_STATE_BRIGHTNESS:
        case SCREEN_SETTING_STATE_FIX:
            _state = SCREEN_SETTING_STATE_MENU;
            menu_close();
            nsec_show_screen_settings();
            break;

        default:
            break;
        }
    }
}
