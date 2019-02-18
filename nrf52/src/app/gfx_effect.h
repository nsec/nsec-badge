//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef gfx_effect_h
#define gfx_effect_h

#include <stdint.h>
#include <stdbool.h>
#include <bitmap.h>

void gfx_set_rotation(uint8_t r);

void nsec_gfx_effect_addNoise(uint8_t noise_amount);

void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour);

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_draw_circle_helper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_fill_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);
void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_fill_screen(uint16_t color);
void gfx_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void gfx_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);

void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void gfx_fill_triangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void gfx_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void gfx_draw_16bit_bitmap(int16_t x, int16_t y, const struct bitmap *bitmap, uint16_t bg_color);
void gfx_draw_bitmap_bg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
void gfx_draw_xbitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

void gfx_write(uint8_t c);
void gfx_putc(char c);
void gfx_puts(const char *s);
void gfx_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

void gfx_set_cursor(int16_t x, int16_t y);
void gfx_set_text_size(uint8_t s);
void gfx_set_text_color(uint16_t c);
void gfx_set_text_background_color(uint16_t c, uint16_t b);
void gfx_set_text_wrap(bool w);

void gfx_update(void);

#endif /* gfx_effect_h */
