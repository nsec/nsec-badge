//  Copyright (c) 2018
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_glue.h"
#include "controls.h"
#include "menu.h"
#include "nsec_led_pattern.h"
#include "nsec_storage.h"
#include "ssd1306.h"
#include "ws2812fx.h"

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_CHOOSE,
    SETTING_STATE_PATTERN,
    SETTING_STATE_BACK,
};

char *letters[] = {
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
};

char pass[4];

static enum setting_state _state = SETTING_STATE_CLOSED;

static void show_basic_pattern_menu(uint8_t item);
static void show_extra_pattern_menu(uint8_t item);
static void save_pattern(uint8_t item);
static void unlock_led_pattern(uint8_t item);

static void save_letter0(void);
static void save_letter1(uint8_t item);
static void save_letter2(uint8_t item);
static void save_letter3(uint8_t item);
static void save_letter4(uint8_t item);

static void led_pattern_handle_buttons(button_t button);

static menu_item_s basic_pattern_items[MODE_BASIC_COUNT];
static menu_item_s extra_pattern_items[MODE_EXTRA_COUNT];
static menu_item_s letters_items[16];
static menu_item_s led_pattern_items[] = {
    {
        .label = "Basic patterns",
        .handler = show_basic_pattern_menu,
    },
    {
        .label = "Extra patterns",
        .handler = unlock_led_pattern,
    }
};

void nsec_led_pattern_show(void) {
    gfx_fillRect(0, 8, 128, 65 - 8, SSD1306_BLACK);
    menu_init(0, 12, 128, 64 - 12, ARRAY_SIZE(led_pattern_items), led_pattern_items);
    nsec_controls_add_handler(led_pattern_handle_buttons);
    _state = SETTING_STATE_MENU;
}

static void save_letter0(void) {
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter1;
    }
    menu_init(18, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter1(uint8_t item) {
    strcpy(pass, (char *) item);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter2;
    }
    menu_init(42, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter2(uint8_t item) {
    strcat(pass, (char *) item);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter3;
    }
    menu_init(66, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter3(uint8_t item) {
    strcat(pass, (char *) item);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter4;
    }
    menu_init(90, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter4(uint8_t item) {
    strcat(pass, (char *) item);
    // TODO check password here
}

static void unlock_led_pattern(uint8_t item) {
    gfx_fillRect(0, 12, 128, 16, SSD1306_BLACK);
    gfx_setCursor(12, 26);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    char brackets[20] = {0};
    snprintf(brackets, 20, "[ ] [ ] [ ] [ ]");
    gfx_puts(brackets);

    save_letter0();
    _state = SETTING_STATE_PATTERN;
}

void show_actual_pattern(void) {
    uint8_t mode = getMode_WS2812FX();
    char actual[50] = {0};

    gfx_fillRect(0, 12, 128, 20, SSD1306_BLACK);
    gfx_setCursor(0, 12);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);

    snprintf(actual, 50, "Now: %s", getModeName_WS2812FX(mode, IS_BASIC_PATTERN));
    gfx_puts(actual);
}

static void show_basic_pattern_menu(uint8_t item) {
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);
    for (int i=0; i<MODE_BASIC_COUNT; i++) {
        basic_pattern_items[i].label = getModeName_WS2812FX(i, IS_BASIC_PATTERN);
        basic_pattern_items[i].handler = save_pattern;
    }
    show_actual_pattern();
    menu_init(0, 32, 128, 64 - 32, ARRAY_SIZE(basic_pattern_items), basic_pattern_items);
    _state = SETTING_STATE_PATTERN;
}

// TODO <L> or <U> before the pattern led
static void show_extra_pattern_menu(uint8_t item) {
    gfx_fillRect(0, 8, 128, 65, SSD1306_BLACK);

    for (int i=0; i<MODE_EXTRA_COUNT; i++) {
        basic_pattern_items[i].label = getModeName_WS2812FX(i, IS_EXTRA_PATTERN);
        basic_pattern_items[i].handler = unlock_led_pattern;
    }
    show_actual_pattern();
    menu_init(0, 32, 128, 64 - 32, ARRAY_SIZE(extra_pattern_items), extra_pattern_items);
    _state = SETTING_STATE_PATTERN;
}

static void save_pattern(uint8_t item) {
    setMode_WS2812FX(item, IS_BASIC_PATTERN);
    update_stored_mode(item);
    show_basic_pattern_menu(0);
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
