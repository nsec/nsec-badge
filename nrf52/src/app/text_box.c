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

#include "text_box.h"
#include "ssd1306.h"
#include "string.h"
#include "controls.h"
#include "utils.h"

// SSD1306_LCDHEIGHT
// SSD1306_LCDWIDTH

#define ROW_COUNT                   8
#define COLUMN_COUNT                21
#define MAX_CHAR        COLUMN_COUNT * ROW_COUNT

typedef struct {
    bool is_at_top;
    bool is_at_bottom;
    bool is_handling_buttons;
    bool is_fullscreen;
    int16_t y_offset;
    int16_t text_index;
    uint16_t text_length;
    const char * text;
} text_box_state_t;

static text_box_state_t text_box;

void text_box_button_handler(button_t button);
void text_box_show_page(void);

void text_box_init(const char *text, bool use_wordwrap, bool use_fullscreen) {
    text_box.is_handling_buttons = true;
    text_box.is_at_top = true;
    text_box.is_at_bottom = false;
    text_box.is_fullscreen = use_fullscreen;
    text_box.text_index = 0;
    text_box.text_length = strlen(text);

    if (use_wordwrap) {
        text_box.text = nsec_word_wrap(text, COLUMN_COUNT);
    } else {
        text_box.text = text;
    }

    nsec_controls_add_handler(text_box_button_handler);

    text_box.y_offset = (use_fullscreen) ? 0 : 16;
    text_box_show_page();
}

void text_box_show_page(void) {
    char buffer[MAX_CHAR] = {0};

    gfx_fillRect(0, 0, 128, 64 - text_box.y_offset, SSD1306_BLACK);
    gfx_setCursor(0, text_box.y_offset);
    if (text_box.is_fullscreen) {
        strncpy(buffer, text_box.text + text_box.text_index, MAX_CHAR);
    } else {
        strncpy(buffer, text_box.text + text_box.text_index, MAX_CHAR - COLUMN_COUNT);
    }

    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void scroll_up(void) {
    if (text_box.is_at_top) {
        return;
    }

    if (text_box.is_at_bottom) {
        //change direction
        text_box.text_index -= MAX_CHAR;
    }

    if (text_box.text_index - MAX_CHAR < 0) {
        text_box.text_index = 0;
        text_box.is_at_top = true;
    } else {
        text_box.text_index -= MAX_CHAR;
        text_box.is_at_bottom = false;
    }

    text_box_show_page();
}

void scroll_down(void) {
    if (text_box.is_at_bottom) {
        return;
    }

    if (text_box.text_index + MAX_CHAR >= text_box.text_length) {
        text_box.text_index = text_box.text_length;
        text_box.is_at_bottom = true;
        return;
    } else {
        text_box.text_index += MAX_CHAR;
        text_box.is_at_top = false;
    }

    text_box_show_page();
}

void text_box_close(void) {
    text_box.is_handling_buttons = 0;
}

void text_box_open(void) {
    text_box.is_handling_buttons = 1;
}

void text_box_button_handler(button_t button) {
    if(text_box.is_handling_buttons) {
        switch (button) {
            case BUTTON_UP:
                if (!text_box.is_at_top) {
                    scroll_up();
                }
                break;
            case BUTTON_DOWN:
                if (!text_box.is_at_bottom) {
                    scroll_down();
                }
                break;
            default:
                break;
        }
    }
}
