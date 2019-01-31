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
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "status_bar.h"
#include "../include/app_glue.h"
#include "drivers/controls.h"

#ifdef BOARD_BRAIN
#define ROW_COUNT                   9 // 10 - status bar
#define COLUMN_COUNT                26  
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT
#else
#define ROW_COUNT                   7 // 8 - status bar
#define COLUMN_COUNT                21  
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT
#endif

static void warning_handle_buttons(button_t button);
static uint16_t text_index = 0;
static bool in_warning_page = false;
const char * warning_notice = "That device is equipped with a protected lithium-ion battery. However, it is still a lithium-ion battery and it should be manipulated with caution. - Do not put the battery under excessive heat or direct sunlight.                  - Do not reverse the battery polarity to avoid damaging your badge.                 - Do not use an unprotected lithium-ion battery into the badge, the badge does not have an under voltage protection. The actual battery in the badge will automatically shutdown when reaching 2.5V. An unprotected battery will drain until 0V and it can be dangerous to recharge afterwards.       - Do not use the battery to power a device that required AA battery. Those batteries are the same size as the one in the badge but the voltage of an AA battery is 1.5V and the voltage of this battery is between 4.2V and 2.5V depending on the charge. You could fry your beautiful TV remote and we don't want that.";

extern uint16_t gfx_width;
extern uint16_t gfx_height;

void show_warning(void) {
    gfx_fill_rect(0, 8, gfx_width, gfx_height - 8, DISPLAY_BLACK);
    gfx_set_cursor(0, 8);

    // We calculate the number of character we can use
    // we keep that value for scrolling
    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    text_index = MAX_CHAR_UNDER_STATUS_BAR;
    strncpy(buffer, warning_notice, text_index);

    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void scroll_up_warning(bool change_direction) {
    text_index -= MAX_CHAR_UNDER_STATUS_BAR;
    if (change_direction) {
        text_index -= MAX_CHAR_UNDER_STATUS_BAR;
    }

    if (text_index <= MAX_CHAR_UNDER_STATUS_BAR) {
        show_warning();
        return;
    }

    gfx_fill_rect(0, 8, gfx_width, gfx_height - 8, DISPLAY_BLACK);
    gfx_set_cursor(0, 8);

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, warning_notice + text_index, MAX_CHAR_UNDER_STATUS_BAR);

    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void scroll_down_warning(bool change_direction) {
    if (text_index > strlen(warning_notice)) {
        return;
    }

    gfx_fill_rect(0, 8, gfx_width, gfx_height - 8, DISPLAY_BLACK);
    gfx_set_cursor(0, 8);   

    if (change_direction) {
        text_index += MAX_CHAR_UNDER_STATUS_BAR;
    }

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, warning_notice + text_index, MAX_CHAR_UNDER_STATUS_BAR);
    text_index += MAX_CHAR_UNDER_STATUS_BAR;

    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_warning_show(void) {
    nsec_controls_add_handler(warning_handle_buttons);
    in_warning_page = true;
    show_warning();
}

static void warning_handle_buttons(button_t button) {
    static button_t last_pressed_button = BUTTON_ENTER;
    if (in_warning_page) {
        if (button == BUTTON_BACK) {
            in_warning_page = false;
            show_main_menu();
        } else if (button == BUTTON_UP) {
               if (last_pressed_button == BUTTON_DOWN) {
                   scroll_up_warning(true);
               } else {
                   scroll_up_warning(false);
               }
            last_pressed_button = BUTTON_UP;
        } else if (button == BUTTON_DOWN) {
            if (last_pressed_button == BUTTON_UP) {
                scroll_down_warning(true);
            } else {
                scroll_down_warning(false);
            }
            last_pressed_button = BUTTON_DOWN;
        }
    }
}
