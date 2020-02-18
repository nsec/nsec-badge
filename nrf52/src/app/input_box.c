//  Copyright (c) 2019
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "input_box.h"
#include "app/gfx_effect.h"
#include "app/home_menu.h"
#include "app/menu.h"
#include "app/utils.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/uart.h"
#include "gui.h"
#include "string.h"

static struct input_box_t {
    uint8_t input_count;
    uint8_t state;
    const char *password;
    const char *input_text;
    bool success;
} input_box;

const char *ib_letters[] = {
    "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", "A", "B", "C", "D", "E", "F",
};

enum chall_state {
    CHALL_STATE_PAUSE,
    CHALL_STATE_SAVING,
};

enum input_box_style {
    STYLE_PASSWORD,
    STYLE_WORD,
};

static menu_item_s letters_items[16];
static void draw_input_box_title(void);
static void input_box_show_brackets(void);
static void input_box_show_cursor(void);
static void is_success(uint8_t item);

void input_box_init(const char *text, const char *password) {
    input_box.input_count = strlen(password);
    if (input_box.input_count > 4) {
        input_box.input_count = 4;
    }
    input_box.password = password;
    input_box.success = true;
    input_box.input_text = text;
    input_box.state = 0;
    draw_input_box_title();
    input_box_show_brackets();
    input_box_show_cursor();
}

static void draw_input_box_title(void) {
    struct title title;
    title.pos_y = 5;
    title.pos_x = 5;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, input_box.input_text);
    draw_title(&title);
}

static void input_box_show_brackets(void)
{
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    char brackets[INPUT_BOX_BRA_WIDTH] = {0};
    for (uint16_t i = 0; i < input_box.input_count; ++i) {
        gfx_set_cursor(INPUT_BOX_BRA_WIDTH * TEXT_BASE_WIDTH * i,
                       INPUT_BOX_POS_Y);
        snprintf(brackets, INPUT_BOX_BRA_WIDTH, " [ ]");
        gfx_puts(brackets);
        gfx_update();
    }
}

static menu_t menu;

static void input_box_show_cursor(void)
{
    for (int i = 0; i < 16; i++) {
        letters_items[i].label = ib_letters[i];
        letters_items[i].handler = is_success;
    }
    menu_close(&menu);
    menu_init(&menu,
              INPUT_BOX_POS_X +
                  INPUT_BOX_BRA_WIDTH * TEXT_BASE_WIDTH * input_box.state,
              INPUT_BOX_POS_Y, TEXT_BASE_HEIGHT, TEXT_BASE_WIDTH,
              ARRAY_SIZE(letters_items), letters_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

static void is_success(uint8_t item) {
    if ((input_box.password[input_box.state]) != *(ib_letters[item])) {
        input_box.success = false;
    }
    if (input_box.state < input_box.input_count - 1) {
        input_box.state += 1;
        input_box_show_cursor();
    } else {
        menu_close(&menu);
        gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
                      DISPLAY_WHITE);
        gfx_set_cursor(GEN_MENU_POS);
        // in the future move the check out of the input_box
        // user can decide to do with the result
        if (input_box.success == true) {
            gfx_puts("Congratz !");
        } else {
            gfx_puts("Try again !");
        }
        gfx_update();
    }
}
