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

// Standard includes.
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Includes from our code.
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include "utils.h"

#define MAX_LINE 250
#define MAX_TEXT_LENGTH 2048

struct text_box {
    struct text_box_config *config;
    bool is_at_top;
    bool is_at_bottom;
    bool is_handling_buttons;
    uint16_t columns;
    uint16_t rows;
    uint16_t line_offset[MAX_LINE];
    uint16_t line_count;
    uint16_t line_index;
    char text[MAX_TEXT_LENGTH];
};

static struct text_box text_box;

static void text_box_button_handler(button_t button);
static void text_box_show_page(void);

void text_box_init(const char *text, struct text_box_config *config)
{
    uint32_t len = strlen(text);
    uint32_t i;

    memset(&text_box, 0, sizeof(text_box));

    text_box.is_handling_buttons = true;
    text_box.is_at_top = true;
    text_box.columns = config->width / TEXT_BASE_WIDTH;
    text_box.rows = (config->height / TEXT_BASE_HEIGHT);
    text_box.config = config;

    strncpy(text_box.text, text, MAX_TEXT_LENGTH);

    // If last charactere is not a \n add it
    if (text_box.text[len - 1] != '\n') {
        text_box.text[len] = '\n';
        text_box.text[len + 1] = '\0';
    }

    word_wrap(text_box.text, text_box.columns);

    // Count the number of lines and assign offset into an array
    text_box.line_count++;
    for (i = 0; i <= len; i++) {
        if (text_box.text[i] == '\n') {
            text_box.line_offset[text_box.line_count] = i + 1;
            text_box.line_count++;
        }
    }

    text_box.is_at_bottom =
        (text_box.line_count <= text_box.rows) ? true : false;

    nsec_controls_add_handler(text_box_button_handler);

    text_box_show_page();
}

static void text_box_show_page(void) {
    uint8_t i, k;
    struct text_box_config *config = text_box.config;
    char buffer[text_box.columns + 1];

    gfx_fill_rect(config->x, config->y, config->width, config->height,
                  config->bg_color);
    gfx_set_cursor(config->x + 1, config->y);
    gfx_set_text_background_color(config->text_color, config->bg_color);
    bool old_wrap_value = gfx_set_text_wrap(false);

    for (i = 0; i < text_box.rows; i++) {
        if (text_box.line_index + i + 1 > text_box.line_count) {
            goto end;
        }

        int16_t line_end = text_box.line_offset[text_box.line_index + i + 1];
        int16_t line_start = text_box.line_offset[text_box.line_index + i];
        int16_t line_size = line_end - line_start;
        if (line_size < 0) {
            text_box.is_at_bottom = true;
            goto end;
        }

        memset(buffer, '\0', text_box.columns + 1);
        strncpy(buffer,
                text_box.text + text_box.line_offset[text_box.line_index + i],
                line_size);
        gfx_puts(buffer);
    }

end:
	gfx_set_text_wrap(old_wrap_value);
}

static void scroll_up(void) {
    if (text_box.is_at_top) {
        return;
    }

    if (text_box.is_at_bottom) {
        text_box.line_index -= text_box.rows;
    }

    if (text_box.line_index - text_box.rows < 0) {
        text_box.line_index = 0;
        text_box.is_at_top = true;
        text_box.is_at_bottom = false;
    } else {
        text_box.line_index -= text_box.rows;
        text_box.is_at_bottom = false;
    }

    text_box_show_page();
}

static void scroll_down(void) {
    if (text_box.is_at_bottom) {
        return;
    }

    if (text_box.line_index + text_box.rows >= text_box.line_count) {
        text_box.line_index = text_box.line_count - text_box.rows;
        text_box.is_at_bottom = true;
        return;
    } else {
        text_box.line_index += text_box.rows;
        text_box.is_at_top = false;
    }

    text_box_show_page();
}

static void text_box_close(void) { text_box.is_handling_buttons = 0; }

static void text_box_button_handler(button_t button) {
    if (text_box.is_handling_buttons) {
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
