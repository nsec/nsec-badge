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
#include "ssd1306.h"
#include "ST7735.h"

struct display_ops {
	void (*init)(void);
	void (*draw_pixel)(int16_t x, int16_t y, uint16_t colour);
	void (*invert_display)(uint8_t i);
	void (*fill_screen_black)(void);
	void (*fill_screen_white)(void);
	void (*draw_fast_hline)(int16_t x, int16_t y, int16_t w, uint16_t color);
	void (*draw_fast_vline)(int16_t x, int16_t y, int16_t h, uint16_t color);
	void (*draw_16bit_bitmap)(int16_t x, int16_t y, const uint8_t *bitmap,
                int16_t w, int16_t h, uint16_t bg_color);
	void (*set_brightness)(uint8_t brightness);
	void (*update)(void);
};

#ifdef BOARD_BRAIN
static struct display_ops st7735_ops =
	{
		&st7735_init,
		&st7735_draw_pixel,
		&st7735_invert_display,
		&st7735_fill_screen_black,
		&st7735_fill_screen_white,
		&st7735_draw_fast_hline,
		&st7735_draw_fast_vline,
		&st7735_draw_16bit_bitmap,
		&st7735_set_brightness,
		NULL
	};
static struct display_ops *ops = &st7735_ops;

#else

static struct display_ops ssd1306_ops =
	{
		&ssd1306_init,
		&ssd1306_draw_pixel,
		&ssd1306_invert_display,
		&ssd1306_fill_screen_black,
		&ssd1306_fill_screen_white,
		&ssd1306_draw_fast_hline,
		&ssd1306_draw_fast_vline,
		NULL,
		NULL,
		&ssd1306_update
	};
static struct display_ops *ops = &ssd1306_ops;
#endif

void display_init(void)
{
	ops->init();
}

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	ops->draw_pixel(x, y, color);
}

void display_invert_display(uint8_t i)
{
	ops->invert_display(i);
}

void display_fill_screen_black(void)
{
	ops->fill_screen_black();
}

void display_fill_screen_white(void)
{
	ops->fill_screen_white();
}

void display_draw_fast_hline(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	ops->draw_fast_hline(x, y, h, color);
}

void display_draw_fast_vline(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	ops->draw_fast_vline(x, y, w, color);
}

void display_draw_16bit_bitmap(int16_t x, int16_t y, const uint8_t *bitmap,
    int16_t w, int16_t h, uint16_t bg_color)
{
	if (ops->draw_16bit_bitmap) {
		ops->draw_16bit_bitmap(x, y, bitmap, w, h, bg_color);
	}
}

void display_set_brightness(uint8_t brightness)
{
	if (ops->set_brightness) {
		ops->set_brightness(brightness);
	}
}

void display_update(void)
{
	if (ops->update) {
		ops->update();
	}
}
