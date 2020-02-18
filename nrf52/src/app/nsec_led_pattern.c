//  Copyright (c) 2018
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <nrf_delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app/gfx_effect.h"
#include "app/home_menu.h"
#include "app/main_menu.h"
#include "app/menu.h"
#include "app/text_box.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gui.h"
#include "led_code_storage.h"
#include "nsec_led_pattern.h"
#include "persistency.h"

#include "nsec_nearby_badges.h"

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_BASIC_PATTERN,
    SETTING_STATE_EXTRA_PATTERN,
    SETTING_STATE_UNLOCK_PATTERN,
};

#define MODE_BASIC_COUNT 41
#define MODE_EXTRA_COUNT 15

static const char *letters[] = {
    "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", "A", "B", "C", "D", "E", "F",
};

static const char *basic_patterns[] = {
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
    // here
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

static uint8_t basic_patterns_match_index[] = {
    0,  1,  2,  3,  4,  5,  6,  9,  13, 14, 15, 16, 18, 19,
    21, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 35, 37,
    40, 41, 43, 44, 45, 46, 47, 48, 49, 51, 52, 53, 55,
};

static const char *extra_patterns_lock[] = {
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
    "<L> Chase Blackout Rainbow",
};

static const char *extra_patterns_unlock[] = {
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
    "<U> Chase Blackout Rainbow",
};

static uint8_t extra_patterns_match_index[] = {
    7, 8, 50, 10, 11, 12, 17, 20, 22, 42, 54, 33, 36, 39, 38,
};

static char pass[5];

static enum setting_state _state = SETTING_STATE_CLOSED;

static void show_basic_pattern_menu(uint8_t item);
static void show_extra_pattern_menu(uint8_t item);
static void set_nearby_mode(uint8_t item);
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
    },
    {
        .label = "Nearby badges",
        .handler = set_nearby_mode,
    }};

static void unlock_all_pattern(void)
{
    uint32_t stored_password = get_stored_pattern_bf();
    if (stored_password != 0x00FFFFFF) {
        stored_password = 0x00FFFFFF;
        update_stored_pattern_bf(stored_password);
    }
}

static void unlock_pattern(uint32_t sponsor_index)
{
    if (sponsor_index < 31) {
        uint32_t stored_password = get_stored_pattern_bf();
        SET_BIT(stored_password, sponsor_index);
        update_stored_pattern_bf(stored_password);
    }
}

//static const char *nsec_get_pattern_pw(uint32_t sponsor_index)
//{
//    if (sponsor_index < SPONSOR_PW_SIZE) {
//        return sponsor_password[sponsor_index];
//    }
//    return 0;
//}

bool pattern_is_unlock(uint32_t sponsor_index)
{
    return IS_SET(get_stored_pattern_bf(), sponsor_index);
}

// true valid, false invalid
static bool nsec_unlock_led_pattern(char *password, uint8_t index)
{
    if (strcmp(password, MASTER_PW) == 0) {
        unlock_all_pattern();
        return true;
    }
    if (strcmp(password, sponsor_password[index]) == 0) {
        if (!pattern_is_unlock(index)) {
            unlock_pattern(index);
        }
        return true;
    }
    return false;
}

static void draw_led_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 25;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "PATTERN");
    draw_title(&title);
}

static void set_nearby_mode(uint8_t item) {
    //select_nearby_badges_pattern();
}

static menu_t menu;

void nsec_led_pattern_show(void)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_led_title();
    menu_init(&menu, GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(led_pattern_items), led_pattern_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    nsec_controls_add_handler(led_pattern_handle_buttons);
    _state = SETTING_STATE_MENU;
}

int8_t get_extra_array_index(uint8_t mode)
{
    for (int i = 0; i < ARRAY_SIZE(extra_patterns_match_index); i++) {
        if (extra_patterns_match_index[i] == mode) {
            return i;
        }
    }
    return -1;
}

static void save_letter0(void)
{
    for (int i = 0; i < 16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter1;
    }
    menu_close(&menu);
    menu_init(&menu, INPUT_BOX_BRA_WIDTH + TEXT_BASE_WIDTH, INPUT_BOX_POS_Y,
              TEXT_BASE_WIDTH, TEXT_BASE_HEIGHT, ARRAY_SIZE(letters_items),
              letters_items, HOME_MENU_BG_COLOR, DISPLAY_WHITE);
}

static void save_letter1(uint8_t item)
{
    strcpy(pass, letters[item]);
    for (int i = 0; i < 16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter2;
    }
    menu_close(&menu);
    menu_init(&menu, INPUT_BOX_BRA_WIDTH * 2 + TEXT_BASE_WIDTH * 4 + 1,
              INPUT_BOX_POS_Y, TEXT_BASE_WIDTH, TEXT_BASE_HEIGHT,
              ARRAY_SIZE(letters_items), letters_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static void save_letter2(uint8_t item)
{
    strcat(pass, letters[item]);
    for (int i = 0; i < 16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = save_letter3;
    }
    menu_close(&menu);
    menu_init(&menu, INPUT_BOX_BRA_WIDTH * 4 + TEXT_BASE_WIDTH * 6 + 3,
              INPUT_BOX_POS_Y, TEXT_BASE_WIDTH, TEXT_BASE_HEIGHT,
              ARRAY_SIZE(letters_items), letters_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static void save_letter3(uint8_t item)
{
    strcat(pass, letters[item]);
    for (int i = 0; i < 16; i++) {
        letters_items[i].label = letters[i];
        letters_items[i].handler = try_unlock;
    }
    menu_close(&menu);
    menu_init(&menu, INPUT_BOX_BRA_WIDTH * 6 + TEXT_BASE_WIDTH * 8 + 5,
              INPUT_BOX_POS_Y, TEXT_BASE_WIDTH, TEXT_BASE_HEIGHT,
              ARRAY_SIZE(letters_items), letters_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static uint8_t index_to_unlock;
static void try_unlock(uint8_t item)
{
    nsec_controls_disable();
    strcat(pass, letters[item]);
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    gfx_set_cursor(GEN_MENU_POS);
    if (nsec_unlock_led_pattern(pass, index_to_unlock)) {
        save_pattern(index_to_unlock);
        gfx_puts("Unlocked !");
    } else {
        gfx_puts("Try again !");
    }
    nrf_delay_ms(1000);
    nsec_controls_enable();
    menu_close(&menu);
    nsec_led_pattern_show();
}

static void unlock_led_pattern(uint8_t item)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    gfx_set_cursor(INPUT_BOX_BRA_WIDTH, INPUT_BOX_POS_Y);
    char brackets[INPUT_BOX_BRA_WIDTH * 4] = {0};
    snprintf(brackets, INPUT_BOX_BRA_WIDTH * 4, "[ ] [ ] [ ] [ ]");
    gfx_puts(brackets);
    index_to_unlock = item;
    _state = SETTING_STATE_UNLOCK_PATTERN;
    menu_close(&menu);
    save_letter0();
}

static void show_actual_pattern(void)
{
    uint8_t mode = getMode_WS2812FX();
    char actual[50] = {0};
    gfx_fill_rect(LED_SET_VAL_POS, GEN_MENU_WIDTH, TEXT_BASE_HEIGHT,
                  DISPLAY_WHITE);
    gfx_set_cursor(LED_SET_VAL_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    snprintf(actual, 50, "%s", getModeName_WS2812FX(mode));
    gfx_puts(actual);
}

static bool basic_selected = false;
static void show_basic_pattern_menu(uint8_t item)
{
    basic_selected = true;
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    for (int i = 0; i < MODE_BASIC_COUNT; i++) {
        basic_pattern_items[i].label = basic_patterns[i];
        basic_pattern_items[i].handler = save_pattern;
    }
    show_actual_pattern();
    menu_init(&menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(basic_pattern_items), basic_pattern_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    _state = SETTING_STATE_BASIC_PATTERN;
}

static void show_extra_pattern_menu(uint8_t item)
{
    basic_selected = false;
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    for (int i = 0; i < MODE_EXTRA_COUNT; i++) {
        if (pattern_is_unlock(i)) {
            extra_pattern_items[i].label = extra_patterns_unlock[i];
            extra_pattern_items[i].handler = save_pattern;
        } else {
            extra_pattern_items[i].label = extra_patterns_lock[i];
            extra_pattern_items[i].handler = unlock_led_pattern;
        }
    }
    menu_close(&menu);
    show_actual_pattern();

    menu_init(&menu, LED_SET_POS, LED_SET_WIDTH, LED_SET_HEIGHT,
              ARRAY_SIZE(extra_pattern_items), extra_pattern_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    _state = SETTING_STATE_EXTRA_PATTERN;
}

static void save_pattern(uint8_t item)
{
    uint8_t index;
    if (basic_selected) {
        index = basic_patterns_match_index[item];
    } else {
        index = extra_patterns_match_index[item];
    }
    setMode_WS2812FX(index);
    update_stored_mode(0, index, true);
    show_actual_pattern();
}

static void led_pattern_handle_buttons(button_t button)
{
    if (button == BUTTON_BACK) {
        switch (_state) {
        case SETTING_STATE_MENU:
            _state = SETTING_STATE_CLOSED;
            menu_close(&menu);
            show_main_menu();
            break;
        case SETTING_STATE_EXTRA_PATTERN:
        case SETTING_STATE_BASIC_PATTERN:
            menu_close(&menu);
            nsec_led_pattern_show();
            break;
        case SETTING_STATE_UNLOCK_PATTERN:
            menu_close(&menu);
            show_extra_pattern_menu(0);
            break;
        default:
            break;
        }
    }
}
