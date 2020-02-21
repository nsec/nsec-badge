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
#include "nsec_led_settings_speed.h"
#include "persistency.h"

#include "FreeRTOS.h"
#include "queue.h"

#define SUPER_SLOW_SPEED_INDEX 0
#define SLOW_SPEED_INDEX 1
#define MEDIUM_SPEED_INDEX 2
#define FAST_SPEED_INDEX 3
#define SUPER_FAST_SPEED_INDEX 4

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static menu_t g_menu;

static void show_actual_speed(void)
{
    uint16_t speed = getSpeed_WS2812FX();
    char actual[50] = {0};
    if (speed >= SUPER_SLOW_SPEED) {
        snprintf(actual, 50, "Now: %s", "Super slow");
    } else if (speed >= SLOW_SPEED) {
        snprintf(actual, 50, "Now: %s", "Slow");
    } else if (speed >= MEDIUM_SPEED) {
        snprintf(actual, 50, "Now: %s", "Medium");
    } else if (speed >= FAST_SPEED) {
        snprintf(actual, 50, "Now: %s", "Fast");
    } else {
        snprintf(actual, 50, "Now: %s", "Super fast");
    }

    gfx_fill_rect(LED_SET_VAL_POS, 6 * 15, 8, DISPLAY_WHITE);
    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts(actual);
}

static void save_speed(uint8_t item)
{
    switch (item) {
    case SUPER_SLOW_SPEED_INDEX:
        setSpeed_WS2812FX(SUPER_SLOW_SPEED);
        break;
    case SLOW_SPEED_INDEX:
        setSpeed_WS2812FX(SLOW_SPEED);
        break;
    case MEDIUM_SPEED_INDEX:
        setSpeed_WS2812FX(MEDIUM_SPEED);
        break;
    case FAST_SPEED_INDEX:
        setSpeed_WS2812FX(FAST_SPEED);
        break;
    case SUPER_FAST_SPEED_INDEX:
        setSpeed_WS2812FX(SUPER_FAST_SPEED);
        break;
    default:
        break;
    }

    update_stored_speed(0, getSpeed_WS2812FX(), true);

    show_actual_speed();
}

static menu_item_s speed_items[] = {
    {
        .label = "Super slow",
        .handler = save_speed,
    },
    {
        .label = "Slow",
        .handler = save_speed,
    },
    {
        .label = "Medium",
        .handler = save_speed,
    },
    {
        .label = "Fast",
        .handler = save_speed,
    },
    {
        .label = "Super fast",
        .handler = save_speed,
    },
};

static void draw_led_title(void)
{
    draw_title("LED CONFIG", 5, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

static void redraw_led_settings_speed(menu_t *menu)
{
    show_actual_speed();

    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    show_actual_speed();
    menu_ui_redraw_all(menu);
}

static bool led_setting_speed_handle_buttons(button_t button, menu_t *menu)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(menu, button);
    }

    return quit;
}

void nsec_show_led_settings_speed(void)
{
    int initial_index = 0;

    menu_init(&g_menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(speed_items), speed_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);

    switch (getSpeed_WS2812FX()) {
    case SUPER_SLOW_SPEED:
        initial_index = SUPER_SLOW_SPEED_INDEX;
        break;
    case SLOW_SPEED:
        initial_index = SLOW_SPEED_INDEX;
        break;
    case MEDIUM_SPEED:
        initial_index = MEDIUM_SPEED_INDEX;
        break;
    case FAST_SPEED:
        initial_index = FAST_SPEED_INDEX;
        break;
    case SUPER_FAST_SPEED:
        initial_index = SUPER_FAST_SPEED_INDEX;
        break;
    }

    menu_set_selected(&g_menu, initial_index);

    redraw_led_settings_speed(&g_menu);

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, portMAX_DELAY);
        APP_ERROR_CHECK_BOOL(ret == pdTRUE);

        bool quit = led_setting_speed_handle_buttons(btn, &g_menu);

        if (quit) {
            break;
        }
    }
}
