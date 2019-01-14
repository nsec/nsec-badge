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

int16_t gfx_width = DISPLAY_WIDTH;
int16_t gfx_height = DISPLAY_HEIGHT;
static int16_t gfx_cursor_y = 0;
static int16_t gfx_cursor_x = 0;
static uint8_t gfx_textsize = 1;
static uint16_t gfx_textcolor = 0xFFFF;
static uint16_t gfx_textbgcolor = 0xFFFF;
static bool gfx_wrap = true;

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

void gfx_set_rotation(uint8_t r)
{
  switch (r) {
   case 0:
     gfx_width  = DISPLAY_WIDTH;
     gfx_height = DISPLAY_HEIGHT;
     break;
   case 1:
     gfx_width  = DISPLAY_HEIGHT;
     gfx_height = DISPLAY_WIDTH;
     break;
  case 2:
     gfx_width  = DISPLAY_WIDTH;
     gfx_height = DISPLAY_HEIGHT;
    break;
   case 3:
     gfx_width  = DISPLAY_HEIGHT;
     gfx_height = DISPLAY_WIDTH;
     break;
  }
}

uint16_t gfx_get_width(void)
{
  return gfx_width;
}

uint16_t gfx_get_height(void)
{
  return gfx_height;
}

void nsec_gfx_effect_addNoise(uint8_t noise_amount) {
    for(int16_t y = 0; y < DISPLAY_HEIGHT; y++) {
        for(int16_t x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t r;
            nsec_random_get(&r, 1);
            if(r < noise_amount) {
            	// TODO do something in mode display instead of reversing the color
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
    display_draw_fast_vline(x, y, h, color);
}

void gfx_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    display_draw_fast_hline(x, y, w, color);
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

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t colour)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  display_draw_pixel(x0  , y0+r, colour);
  display_draw_pixel(x0  , y0-r, colour);
  display_draw_pixel(x0+r, y0  , colour);
  display_draw_pixel(x0-r, y0  , colour);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    display_draw_pixel(x0 + x, y0 + y, colour);
    display_draw_pixel(x0 - x, y0 + y, colour);
    display_draw_pixel(x0 + x, y0 - y, colour);
    display_draw_pixel(x0 - x, y0 - y, colour);
    display_draw_pixel(x0 + y, y0 + x, colour);
    display_draw_pixel(x0 - y, y0 + x, colour);
    display_draw_pixel(x0 + y, y0 - x, colour);
    display_draw_pixel(x0 - y, y0 - x, colour);
  }
}

void gfx_draw_circle_helper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t colour)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      display_draw_pixel(x0 + x, y0 + y, colour);
      display_draw_pixel(x0 + y, y0 + x, colour);
    }
    if (cornername & 0x2) {
      display_draw_pixel(x0 + x, y0 - y, colour);
      display_draw_pixel(x0 + y, y0 - x, colour);
    }
    if (cornername & 0x8) {
      display_draw_pixel(x0 - y, y0 + x, colour);
      display_draw_pixel(x0 - x, y0 + y, colour);
    }
    if (cornername & 0x1) {
      display_draw_pixel(x0 - y, y0 - x, colour);
      display_draw_pixel(x0 - x, y0 - y, colour);
    }
  }
}

void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t colour)
{
  gfx_draw_fast_vline(x0, y0-r, 2*r+1, colour);
  gfx_fill_circle_helper(x0, y0, r, 3, 0, colour);
}

void gfx_fill_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t colour)
{

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      gfx_draw_fast_vline(x0+x, y0-y, 2*y+1+delta, colour);
      gfx_draw_fast_vline(x0+y, y0-x, 2*x+1+delta, colour);
    }
    if (cornername & 0x2) {
      gfx_draw_fast_vline(x0-x, y0-y, 2*y+1+delta, colour);
      gfx_draw_fast_vline(x0-y, y0-x, 2*x+1+delta, colour);
    }
  }
}

void gfx_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour)
{
  gfx_draw_fast_hline(x+r  , y    , w-2*r, colour);
  gfx_draw_fast_hline(x+r  , y+h-1, w-2*r, colour);
  gfx_draw_fast_vline(x    , y+r  , h-2*r, colour);
  gfx_draw_fast_vline(x+w-1, y+r  , h-2*r, colour);
  gfx_draw_circle_helper(x+r    , y+r    , r, 1, colour);
  gfx_draw_circle_helper(x+w-r-1, y+r    , r, 2, colour);
  gfx_draw_circle_helper(x+w-r-1, y+h-r-1, r, 4, colour);
  gfx_draw_circle_helper(x+r    , y+h-r-1, r, 8, colour);
}

void gfx_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h,
        int16_t r, uint16_t colour)
{
  gfx_fill_rect(x+r, y, w-2*r, h, colour);
  gfx_fill_circle_helper(x+w-r-1, y+r, r, 1, h-2*r-1, colour);
  gfx_fill_circle_helper(x+r    , y+r, r, 2, h-2*r-1, colour);
}

void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t colour)
{
  gfx_draw_line(x0, y0, x1, y1, colour);
  gfx_draw_line(x1, y1, x2, y2, colour);
  gfx_draw_line(x2, y2, x0, y0, colour);
}

void gfx_fill_triangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
          int16_t x2, int16_t y2, uint16_t colour)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    gfx_draw_fast_hline(a, y0, b-a+1, colour);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b) swap(a,b);
    gfx_draw_fast_hline(a, y, b-a+1, colour);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b) swap(a,b);
    gfx_draw_fast_hline(a, y, b-a+1, colour);
  }
}

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t colour) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  display_draw_pixel(x0  , y0+r, colour);
  display_draw_pixel(x0  , y0-r, colour);
  display_draw_pixel(x0+r, y0  , colour);
  display_draw_pixel(x0-r, y0  , colour);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    display_draw_pixel(x0 + x, y0 + y, colour);
    display_draw_pixel(x0 - x, y0 + y, colour);
    display_draw_pixel(x0 + x, y0 - y, colour);
    display_draw_pixel(x0 - x, y0 - y, colour);
    display_draw_pixel(x0 + y, y0 + x, colour);
    display_draw_pixel(x0 - y, y0 + x, colour);
    display_draw_pixel(x0 + y, y0 - x, colour);
    display_draw_pixel(x0 - y, y0 - x, colour);
  }
}

void gfx_draw_circle_helper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t colour) 
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      display_draw_pixel(x0 + x, y0 + y, colour);
      display_draw_pixel(x0 + y, y0 + x, colour);
    } 
    if (cornername & 0x2) {
      display_draw_pixel(x0 + x, y0 - y, colour);
      display_draw_pixel(x0 + y, y0 - x, colour);
    }
    if (cornername & 0x8) {
      display_draw_pixel(x0 - y, y0 + x, colour);
      display_draw_pixel(x0 - x, y0 + y, colour);
    }
    if (cornername & 0x1) {
      display_draw_pixel(x0 - y, y0 - x, colour);
      display_draw_pixel(x0 - x, y0 - y, colour);
    }
  }
}

void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t colour) 
{
  gfx_draw_fast_vline(x0, y0-r, 2*r+1, colour);
  gfx_fill_circle_helper(x0, y0, r, 3, 0, colour);
}

void gfx_fill_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, 
      int16_t delta, uint16_t colour) 
{

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      gfx_draw_fast_vline(x0+x, y0-y, 2*y+1+delta, colour);
      gfx_draw_fast_vline(x0+y, y0-x, 2*x+1+delta, colour);
    }
    if (cornername & 0x2) {
      gfx_draw_fast_vline(x0-x, y0-y, 2*y+1+delta, colour);
      gfx_draw_fast_vline(x0-y, y0-x, 2*x+1+delta, colour);
    }
  }
}

void draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour) 
{
  gfx_draw_fast_hline(x+r  , y    , w-2*r, colour); 
  gfx_draw_fast_hline(x+r  , y+h-1, w-2*r, colour);
  gfx_draw_fast_vline(x    , y+r  , h-2*r, colour);
  gfx_draw_fast_vline(x+w-1, y+r  , h-2*r, colour);
  gfx_draw_circle_helper(x+r    , y+r    , r, 1, colour);
  gfx_draw_circle_helper(x+w-r-1, y+r    , r, 2, colour);
  gfx_draw_circle_helper(x+w-r-1, y+h-r-1, r, 4, colour);
  gfx_draw_circle_helper(x+r    , y+h-r-1, r, 8, colour);
}

void fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, 
        int16_t r, uint16_t colour) 
{
  gfx_fill_rect(x+r, y, w-2*r, h, colour);
  gfx_fill_circle_helper(x+w-r-1, y+r, r, 1, h-2*r-1, colour);
  gfx_fill_circle_helper(x+r    , y+r, r, 2, h-2*r-1, colour);
}

void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t colour) 
{
  gfx_draw_line(x0, y0, x1, y1, colour);
  gfx_draw_line(x1, y1, x2, y2, colour);
  gfx_draw_line(x2, y2, x0, y0, colour);
}

void gfx_fill_triangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
          int16_t x2, int16_t y2, uint16_t colour) 
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    gfx_draw_fast_hline(a, y0, b-a+1, colour);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b) swap(a,b);
    gfx_draw_fast_hline(a, y, b-a+1, colour);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b) swap(a,b);
    gfx_draw_fast_hline(a, y, b-a+1, colour);
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
