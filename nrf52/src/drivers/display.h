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

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <nrf.h>

#define swap(a, b)                                                             \
    {                                                                          \
        int16_t t = a;                                                         \
        a = b;                                                                 \
        b = t;                                                                 \
    }

#ifdef BOARD_BRAIN
// Size
#define DISPLAY_HEIGHT 160
#define DISPLAY_WIDTH 80

// Colors
#define DISPLAY_BLACK 0x0000
#define DISPLAY_RED 0xF800
#define DISPLAY_GREEN 0x07E0
#define DISPLAY_BLUE 0x001F
#define DISPLAY_CYAN 0x07FF
#define DISPLAY_MAGENTA 0xF81F
#define DISPLAY_YELLOW 0xFFE0
#define DISPLAY_ORANGE 0xFC00
#define DISPLAY_WHITE 0xFFFF

#else
// Size
#define DISPLAY_HEIGHT 128
#define DISPLAY_WIDTH 64

// Colors
#define DISPLAY_BLACK 0
#define DISPLAY_RED 1
#define DISPLAY_BLUE 1
#define DISPLAY_GREEN 1
#define DISPLAY_CYAN 1
#define DISPLAY_MAGENTA 1
#define DISPLAY_YELLOW 1
#define DISPLAY_WHITE 1
#endif

// Function
void display_init(void);
void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_invert_display(uint8_t i);
void display_fill_screen_black(void);
void display_fill_screen_white(void);
void display_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);
void display_draw_fast_vline(int16_t x, int16_t y, int16_t w, uint16_t color);
void display_draw_16bit_bitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                               int16_t w, int16_t h, uint16_t bg_color);
void display_update(void);
void display_set_brightness(uint8_t brightness);

#endif //_DISPLAY_H