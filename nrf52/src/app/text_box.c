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
#include "drivers/display.h"
#include "gfx_effect.h"
#include "string.h"
#include "drivers/controls.h"
#include "utils.h"
#include "gui.h"

typedef struct {
    bool is_at_top;
    bool is_at_bottom;
    bool is_handling_buttons;
    uint16_t columns;
    uint16_t rows;
    uint16_t max_char;
    struct text_box_config *config;
    int16_t text_index;
    uint16_t text_length;
    const char * text;
} text_box_state_t;

static text_box_state_t text_box;

void text_box_button_handler(button_t button);
void text_box_show_page(void);

void text_box_init(const char *text, struct text_box_config *config)
{
    text_box.is_handling_buttons = true;
    text_box.is_at_top = true;
    text_box.is_at_bottom = false;
    text_box.text_index = 0;
    text_box.text_length = strlen(text);
    text_box.text = text;
    text_box.columns = config->width / TEXT_BASE_WIDTH;
    text_box.rows = config->height / TEXT_BASE_HEIGHT;
    text_box.max_char = text_box.columns * text_box.rows;
    text_box.config = config;

    nsec_controls_add_handler(text_box_button_handler);

    gfx_set_wrap_line(config->width);

    text_box_show_page();
}

void text_box_show_page(void) {
    struct text_box_config *config = text_box.config;
    char buffer[text_box.max_char];

    memset(buffer, 0, text_box.max_char);

    strncpy(buffer, text_box.text + text_box.text_index, text_box.max_char);

    gfx_fill_rect(config->x, config->y, config->width, config->height, config->bg_color);
    gfx_set_cursor(config->x, config->y);
    gfx_set_text_background_color(config->text_color, config->bg_color);
    gfx_puts(buffer);
    gfx_update();
}

void scroll_up(void) {
    if (text_box.is_at_top) {
        return;
    }

    if (text_box.is_at_bottom) {
        //change direction
        text_box.text_index -= text_box.max_char;
    }

    if (text_box.text_index - text_box.max_char < 0) {
        text_box.text_index = 0;
        text_box.is_at_top = true;
    } else {
        text_box.text_index -= text_box.max_char;
        text_box.is_at_bottom = false;
    }

    text_box_show_page();
}

void scroll_down(void) {
    if (text_box.is_at_bottom) {
        return;
    }

    if (text_box.text_index + text_box.max_char >= text_box.text_length) {
        text_box.text_index = text_box.text_length;
        text_box.is_at_bottom = true;
        return;
    } else {
        text_box.text_index += text_box.max_char;
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
            case BUTTON_BACK:
                text_box_close();
            default:
                break;
        }
    }
}
