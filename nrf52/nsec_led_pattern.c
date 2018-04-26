//  Copyright (c) 2018
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <stdbool.h>
#include <stdio.h>

#include "app_glue.h"
#include "controls.h"
#include "menu.h"
#include "nsec_led_pattern.h"
// #include "nsec_led_settings.h"
#include "nsec_storage.h"
#include "ssd1306.h"
#include "ws2812fx.h"

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_CHOOSE,
    SETTING_STATE_UNLOCK,
    SETTING_STATE_PATTERN,
};

static enum setting_state _state = SETTING_STATE_CLOSED;

static void show_led_pattern_menu(uint8_t item);
static void save_pattern(uint8_t item);

static void led_pattern_handle_buttons(button_t button);

static menu_item_s pattern_items[MODE_COUNT];
static menu_item_s led_pattern_items[] = {
     {
        .label = "Led pattern",
        .handler = show_led_pattern_menu,
    }
};

void nsec_led_pattern_show(void) {
    gfx_fillRect(0, 8, 128, 65 - 8, SSD1306_BLACK);
    menu_init(0, 12, 128, 64 - 12, sizeof(led_pattern_items) / sizeof(led_pattern_items[0]), led_pattern_items);
    nsec_controls_add_handler(led_pattern_handle_buttons);
    _state = SETTING_STATE_MENU;
}

void show_actual_pattern(void) {
    uint8_t mode = getMode_WS2812FX();
    char actual[50] = {0};

    gfx_fillRect(12, 20, 128, 65, SSD1306_BLACK);
    gfx_setCursor(0, 12);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);

    snprintf(actual, 50, "Now: %s", getModeName_WS2812FX(mode));
    gfx_puts(actual);
}

static void show_led_pattern_menu(uint8_t item) {
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);

    for (int i=0; i<MODE_COUNT; i++) {
        pattern_items[i].label = getModeName_WS2812FX(i);
        pattern_items[i].handler = save_pattern;
    }
    show_actual_pattern();
    menu_init(0, 32, 128, 64 - 32, ARRAY_SIZE(pattern_items), pattern_items);
    _state = SETTING_STATE_PATTERN;
}

static void save_pattern(uint8_t item) {
    setMode_WS2812FX(item);
    update_stored_mode(item);
    show_led_pattern_menu(0);
}

static void led_pattern_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (_state) {
            case SETTING_STATE_MENU:
                _state = SETTING_STATE_CLOSED;
                menu_close();
                show_main_menu();
                break;
            case SETTING_STATE_PATTERN:
                _state = SETTING_STATE_MENU;
                menu_close();
                nsec_led_pattern_show();
                break;
            default:
                break;
        }
    }
}
