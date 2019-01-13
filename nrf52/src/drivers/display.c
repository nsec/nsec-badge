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
		&st7735_update
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

void display_update(void)
{
	ops->update();
}


/*
void ssd1306_command(uint8_t c);
void ssd1306_startscrollright(uint8_t start, uint8_t stop);
void ssd1306_startscrollleft(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagright(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop);
void ssd1306_stopscroll(void);
void ssd1306_dim(bool dim);
void ssd1306_update(void);
void ssd1306_clearDisplay(void);
void ssd1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void ssd1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);
void ssd1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ssd1306_drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color);
*/