//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <string.h>
#include "gfx_effect.h"
#include "drivers/display.h"
#include "random.h"
#include "images/font_bitmap.c"

static int16_t gfx_width = DISPLAY_WIDTH;
static int16_t gfx_height = DISPLAY_HEIGHT;
static int16_t gfx_cursor_y = 0;
static int16_t gfx_cursor_x = 0;
static uint8_t gfx_textsize = 1;
static uint16_t gfx_textcolor = 0xFFFF;
static uint16_t gfx_textbgcolor = 0xFFFF;
static bool gfx_wrap = true;

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

void nsec_gfx_effect_addNoise(uint8_t noise_amount) {
    for(int16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        for(int16_t x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t r;
            nsec_random_get(&r, 1);
            if(r < noise_amount) {
            	// TODO do something in mode st7735 instead of reversing the color
                display_draw_pixel(x, y, 2);
            }
        }
    }
}

void gfx_update(void) {
	display_update();
}

/* Bresenham's algorithm */
void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      display_draw_pixel(y0, x0, colour);
    } else {
      display_draw_pixel(x0, y0, colour);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


void gfx_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    gfx_draw_line(x, y, x, y+h-1, color);
}

void gfx_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    gfx_draw_line(x, y, x+w-1, y, color);
}

// Draw a rectangle
void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    gfx_draw_fast_hline(x, y, w, color);
    gfx_draw_fast_hline(x, y+h-1, w, color);
    gfx_draw_fast_vline(x, y, h, color);
    gfx_draw_fast_vline(x+w-1, y, h, color);
}

void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    for (int16_t i=x; i<x+w; i++) {
        gfx_draw_fast_vline(i, y, h, color);
    }
}

void gfx_fill_screen(uint16_t color) {
    if (color == DISPLAY_BLACK) {
    	display_fill_screen_black();
    }
    else if(color == DISPLAY_WHITE) {
    	display_fill_screen_white();
    }
    else {
        gfx_fill_rect(0, 0, gfx_width, gfx_height, color);
    }
}

void gfx_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        display_draw_pixel(x+i, y+j, color);
      }
    }
  }
}

// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
void gfx_draw_bitmap_bg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        display_draw_pixel(x+i, y+j, color);
      }
      else {
        display_draw_pixel(x+i, y+j, bg);
      }
    }
  }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void gfx_draw_xbitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        display_draw_pixel(x+i, y+j, color);
      }
    }
  }
}

// Draw a character
void gfx_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if((x >= gfx_width)            || // Clip right
     (y >= gfx_height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font_bitmap+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1){ // default size
          display_draw_pixel(x+i, y+j, color);
        }
        else {  // big size
          gfx_fill_rect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          display_draw_pixel(x+i, y+j, bg);
        else {  // big size
          gfx_fill_rect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void gfx_write(uint8_t c) {
  if (c == '\n') {
    gfx_cursor_y += gfx_textsize*8;
    gfx_cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    gfx_draw_char(gfx_cursor_x, gfx_cursor_y, c, gfx_textcolor, gfx_textbgcolor, gfx_textsize);
    gfx_cursor_x += gfx_textsize*6;
    if (gfx_wrap && (gfx_cursor_x > (gfx_width - gfx_textsize*6))) {
      gfx_cursor_y += gfx_textsize*8;
      gfx_cursor_x = 0;
    }
  }
}

void gfx_putc(char c) {
    gfx_write((uint8_t)c);
}

void gfx_puts(const char *s) {
    while (*s) {
        gfx_write((uint8_t)*s++);
    }
}

void gfx_set_cursor(int16_t x, int16_t y) {
    gfx_cursor_x = x;
    gfx_cursor_y = y;
}

void gfx_set_text_size(uint8_t s) {
    gfx_textsize = (s > 0) ? s : 1;
}

void gfx_set_text_color(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    gfx_textcolor = gfx_textbgcolor = c;
}

void gfx_set_text_background_color(uint16_t c, uint16_t b) {
    gfx_textcolor   = c;
    gfx_textbgcolor = b;
}

void gfx_set_text_wrap(bool w) {
    gfx_wrap = w;
}
