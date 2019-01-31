//  Copyright (c) 2018
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <nrf_delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_glue.h"
#include "drivers/controls.h"
#include "menu.h"
#include "nsec_led_pattern.h"
#include "drivers/nsec_storage.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "drivers/ws2812fx.h"
#include "nsec_led_ble.h"

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_BASIC_PATTERN,
    SETTING_STATE_EXTRA_PATTERN,
    SETTING_STATE_UNLOCK_PATTERN,
};

#define MODE_BASIC_COUNT 42
#define MODE_EXTRA_COUNT 14

extern uint16_t gfx_width;
extern uint16_t gfx_height;

char *letters[] = {"0", "1", "2", "3", "4", "5", "6", "7",
                    "8", "9", "A", "B", "C", "D", "E", "F",};

const char *basic_patterns[] = {
    "Static",
    "Blink",
    "Breath",
    "Color Wipe",
    "Color Wipe Inverse",
    "Color Wipe Reverse",
    "Color Wipe Reverse Inverse",
    "Single Dynamic",
    "Scan",
    "Dual Scan",
    "Fade",
    "Theater Chase",
    "Running Lights",
    "Twinkle",
    "Twinkle Fade",
    "Sparkle",
    "Flash Sparkle",
    "Hyper Sparkle",
    "Strobe",
    "Strobe Rainbow",
    "Multi Strobe",
    "Blink Rainbow",
    "Chase White",
    "Chase Color",
    "Chase Random",
    "Chase Flash",
    "Chase Flash Random",
    "Chase Blackout",
    "Chase Blackout Rainbow",
    "Running Color",
    "Running Red Blue",
    "Larson Scanner",
    "Comet",
    "Fireworks",
    "Fireworks Random",
    "Merry Christmas",
    "Fire Flicker",
    "Fire Flicker (soft)",
    "Circus Combustus",
    "Halloween",
    "Bicolor Chase",
    "ICU",
};

uint8_t basic_patterns_match_index[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    9,
    13,
    14,
    15,
    16,
    18,
    19,
    21,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    34,
    35,
    37,
    38,
    40,
    41,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    51,
    52,
    53,
    55,
};

const char *extra_patterns_lock[] = {
    "<L> Color Wipe Random",
    "<L> Random Color",
    "<L> Fire Flicker (Intense)",
    "<L> Multi Dynamic",
    "<L> Rainbow",
    "<L> Rainbow Cycle",
    "<L> Theater Chase Rainbow",
    "<L> Twinkle Random",
    "<L> Twinkle Fade Random",
    "<L> Running Random",
    "<L> Tricolor Chase",
    "<L> Chase Rainbow",
    "<L> Chase Rainbow White",
    "<L> Color Sweep Random",
};

const char *extra_patterns_unlock[] = {
    "<U> Color Wipe Random",
    "<U> Random Color",
    "<U> Fire Flicker (Intense)",
    "<U> Multi Dynamic",
    "<U> Rainbow",
    "<U> Rainbow Cycle",
    "<U> Theater Chase Rainbow",
    "<U> Twinkle Random",
    "<U> Twinkle Fade Random",
    "<U> Running Random",
    "<U> Tricolor Chase",
    "<U> Chase Rainbow",
    "<U> Chase Rainbow White",
    "<U> Color Sweep Random",
};

uint8_t extra_patterns_match_index[] = {
    7,
    8,
    50,
    10,
    11,
    12,
    17,
    20,
    22,
    42,
    54,
    33,
    36,
    39,
};


char pass[5];

static enum setting_state _state = SETTING_STATE_CLOSED;

static void show_basic_pattern_menu(uint8_t item);
static void show_extra_pattern_menu(uint8_t item);
static void save_pattern(uint8_t item);
static void unlock_led_pattern(uint8_t item);

static void save_letter0(void);
static void save_letter1(uint8_t item);
static void save_letter2(uint8_t item);
static void save_letter3(uint8_t item);
static void try_unlock(uint8_t item);

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
        .handler = show_extra_pattern_menu,
    }
};

void nsec_led_pattern_show(void) {
    gfx_fill_rect(0, 8, gfx_width, gfx_height - 8, DISPLAY_BLACK);
    menu_init(0, 12, gfx_width, gfx_height - 12, ARRAY_SIZE(led_pattern_items), led_pattern_items);
    nsec_controls_add_handler(led_pattern_handle_buttons);
    _state = SETTING_STATE_MENU;
}

int8_t get_extra_array_index(uint8_t mode) {
    for (int i = 0; i < ARRAY_SIZE(extra_patterns_match_index); i++) {
        if (extra_patterns_match_index[i] == mode) {
            return i;
        }
    }
    return -1;
}

static void save_letter0(void) {
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter1;
    }
    menu_close();
    menu_init(18, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter1(uint8_t item) {
    strcpy(pass, letters[item]);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter2;
    }
    menu_close();
    menu_init(42, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter2(uint8_t item) {
    strcat(pass, letters[item]);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter3;
    }
    menu_close();
    menu_init(66, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static void save_letter3(uint8_t item) {
    strcat(pass, letters[item]);
    for (int i=0; i<16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = try_unlock;
    }
    menu_close();
    menu_init(90, 26, 8, 4, ARRAY_SIZE(letters_items), letters_items);
}

static uint8_t index_to_unlock;
static void try_unlock(uint8_t item) {
    strcat(pass, letters[item]);
    gfx_fill_rect(0, 12, gfx_width, gfx_height-12, DISPLAY_BLACK);
    gfx_set_cursor(12, 26);
    if (nsec_unlock_led_pattern(pass, index_to_unlock)) {
        save_pattern(index_to_unlock);
        gfx_puts("Unlocked !");
    } else {
        gfx_puts("Try again !");
    }
    gfx_update();
    nrf_delay_ms(1000);
    menu_close();
    nsec_led_pattern_show();
}

static void unlock_led_pattern(uint8_t item) {
    gfx_fill_rect(0, 12, gfx_width, gfx_height-12, DISPLAY_BLACK);
    gfx_set_cursor(12, 26);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    char brackets[20] = {0};
    snprintf(brackets, 20, "[ ] [ ] [ ] [ ]");
    gfx_puts(brackets);
    index_to_unlock = item;
    _state  = SETTING_STATE_UNLOCK_PATTERN;
    menu_close();
    save_letter0();
}

void show_actual_pattern(void) {
    uint8_t mode = getMode_WS2812FX();
    char actual[50] = {0};

    gfx_fill_rect(0, 12, gfx_width, 20, DISPLAY_BLACK);
    gfx_set_cursor(0, 12);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);

    snprintf(actual, 50, "Now: %s", getModeName_WS2812FX(mode));
    gfx_puts(actual);
    gfx_update();
}

static bool basic_selected = false;
static void show_basic_pattern_menu(uint8_t item) {
    basic_selected = true;
    gfx_fill_rect(0, 8, gfx_width, 65, DISPLAY_BLACK);
    for (int i=0; i < MODE_BASIC_COUNT; i++) {
        basic_pattern_items[i].label = basic_patterns[i];
        basic_pattern_items[i].handler = save_pattern;
    }
    show_actual_pattern();
    menu_init(0, 32, gfx_width, gfx_height - 32, ARRAY_SIZE(basic_pattern_items), basic_pattern_items);
    _state = SETTING_STATE_BASIC_PATTERN;
}

static void show_extra_pattern_menu(uint8_t item) {
    basic_selected = false;
    gfx_fill_rect(0, 8, gfx_width, 65, DISPLAY_BLACK);

    for (int i=0; i<MODE_EXTRA_COUNT; i++) {
        if (pattern_is_unlock(i)) {
            extra_pattern_items[i].label = extra_patterns_unlock[i];
            extra_pattern_items[i].handler = save_pattern;
        } else {
            extra_pattern_items[i].label = extra_patterns_lock[i];
            extra_pattern_items[i].handler = unlock_led_pattern;
        }
    }
    menu_close();
    show_actual_pattern();
    menu_init(0, 32, gfx_width, gfx_height - 32, ARRAY_SIZE(extra_pattern_items), extra_pattern_items);
    _state = SETTING_STATE_EXTRA_PATTERN;
}

static void save_pattern(uint8_t item) {
    uint8_t index;
    if (basic_selected) {
        index = basic_patterns_match_index[item];
    } else {
        index = extra_patterns_match_index[item];
    }

    menu_take_over();
    setMode_WS2812FX(index);
    update_stored_mode(index);
    show_actual_pattern();
}

static void led_pattern_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (_state) {
            case SETTING_STATE_MENU:
                _state = SETTING_STATE_CLOSED;
                menu_close();
                show_main_menu();
                break;
            case SETTING_STATE_EXTRA_PATTERN:
            case SETTING_STATE_BASIC_PATTERN:
                menu_close();
                nsec_led_pattern_show();
                break;
            case SETTING_STATE_UNLOCK_PATTERN:
                menu_close();
                show_extra_pattern_menu(0);
                break;
            default:
                break;
        }
    }
}
