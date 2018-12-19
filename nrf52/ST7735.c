/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  based on port to tm4c123gh6pm by Julian Fell.
  Ported to nrf52 by Eric Tremblay.
 ****************************************************/
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <app_util_platform.h>
#include <stdlib.h>
#include "ST7735.h"

// This increase the bss section by 25k !!!!
#define USE_DOUBLE_BUFFERING

//*****************************************************************************
//
// Local Defines
//
//*****************************************************************************

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 160

#define DELAY 0x80

#define SET_LOW(x) nrf_gpio_pin_write(x, 0)
#define SET_HIGH(x) nrf_gpio_pin_write(x, 1)

#define swap(a, b) { int16_t t = a; a = b; b = t; }

typedef enum {
    COMMAND = 0,
    DATA = 1,
} t_screen_mode;

//*****************************************************************************
//
// Private Variables
//
//*****************************************************************************

int8_t colstart, rowstart, rotation, textsize;
int16_t wrap,cursor_y,cursor_x;
uint16_t textcolour,textbgcolour;
uint32_t width, height;
static bool is_init = false;

//*****************************************************************************
//
// SPI stuff
//
//*****************************************************************************
static st7735_config_t st7735_config;

static void spi_init(void) {
    nrf_drv_spi_config_t spi_config;

    spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
    spi_config.sck_pin = st7735_config.sck_pin;
    spi_config.miso_pin = st7735_config.miso_pin;
    spi_config.mosi_pin = st7735_config.mosi_pin;
    spi_config.ss_pin = st7735_config.cs_pin;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.irq_priority = APP_IRQ_PRIORITY_LOW;
    spi_config.orc = 0xFF;

    APP_ERROR_CHECK(nrf_drv_spi_init(&st7735_config.spi, &spi_config, NULL, NULL));
}

static void spi_master_tx(const uint8_t * p_tx_data, uint16_t len) {
    while(len > 0) {
        const uint8_t packet_len = MIN(len, UINT8_MAX);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&st7735_config.spi, p_tx_data, packet_len, NULL, 0));
        len -= packet_len;
        p_tx_data += packet_len;
    }
}

static void spi_write(uint8_t byte) {
    spi_master_tx(&byte, 1);
}

static void st7735_command(uint8_t c) {
    nrf_gpio_pin_write(st7735_config.dc_pin, COMMAND);
    spi_write(c);
}

static void st7735_data(uint8_t d) {
    nrf_gpio_pin_write(st7735_config.dc_pin, DATA);
    spi_write(d);
}


//*****************************************************************************
//
// Screen Initialisation
//
//*****************************************************************************

#ifdef USE_DOUBLE_BUFFERING
static uint8_t buffer[SCREEN_HEIGHT * SCREEN_WIDTH * 2] = {0};
#endif

static const uint8_t
  initCommands[] = {                 // Init for 7735R, (green tab)
    8,                       // 8 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay

    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay

    ST7735_COLMOD , 1      ,  // 15: set colour mode, 1 arg, no delay:
      0x05 ,                 //     16-bit colour

    ST7735_MADCTL  ,  DELAY,  
      255,                    //     500 ms delay

    ST7735_MADCTL , 1      ,  
      0xc8 ,                 
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

// Companion code to the above table.  Reads and issues
// a series of LCD commands stored in byte array.
void command_list(const uint8_t *addr) 
{
    uint8_t  numCommands, numArgs;
    uint16_t ms;

    numCommands = *(addr++);   // Number of commands to follow

    while(numCommands--) {                // For each command...
        st7735_command(*(addr++)); //   Read, issue command
        numArgs  = *(addr++);    //   Number of args to follow
        ms       = numArgs & DELAY;          //   If hibit set, delay follows args
        numArgs &= ~DELAY;                   //   Mask out delay bit

        while(numArgs--) {                   //   For each argument...
            st7735_data(*(addr++));  //     Read, issue argument
        }

        if(ms) {
            ms = *(addr++); // Read post-command delay time (ms)
            if (ms == 255) {
                ms = 500;     // If 255, delay for 500 ms
            } 
            nrf_delay_ms(ms);
        }

    }
}

void st7735_init(st7735_config_t init_st7735_config) 
{
    if (is_init) {
        return;
    }

    st7735_config = init_st7735_config;

    spi_init();

    /* Set st7735_config.dc_pin and RST Pins as outputs for manual toggling */
    nrf_gpio_cfg_output(st7735_config.dc_pin);
    nrf_gpio_cfg_output(st7735_config.rst_pin);

    /* Toggle reset */
    SET_HIGH(st7735_config.rst_pin);
    nrf_delay_ms(500);
    SET_LOW(st7735_config.rst_pin);
    nrf_delay_ms(500);
    SET_HIGH(st7735_config.rst_pin);
    nrf_delay_ms(500);

    /* Initialise default values */
    colstart = 0;
    rowstart = 0;
    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;
    wrap = 1;
    cursor_y = 0;
    cursor_x = 0;
    textsize = 1;
    textcolour = 0xFFFF;
    textbgcolour = 0xFFFF;

    /* Initialise LCD screen */
    command_list(initCommands);

    is_init = true;
}

//*****************************************************************************
//
// Basic Drawing Functions
//
//*****************************************************************************

uint16_t swap_colour(uint16_t x) 
{ 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

void st7735_update(void)
{
#ifdef USE_DOUBLE_BUFFERING
    st7735_command(ST7735_CASET); 
    st7735_data(0x00);
    st7735_data(colstart);   
    st7735_data(0x00);
    st7735_data(SCREEN_WIDTH + colstart - 1);  

    st7735_command(ST7735_RASET); 
    st7735_data(0x00);
    st7735_data(rowstart);  
    st7735_data(0x00);
    st7735_data(SCREEN_HEIGHT + rowstart - 1); 
    st7735_command(ST7735_RAMWR);

    SET_HIGH(st7735_config.dc_pin);
    spi_master_tx(buffer, SCREEN_HEIGHT * SCREEN_WIDTH * 2);
#endif
}

void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
  st7735_command(ST7735_CASET); 
  st7735_data(0x00);
  st7735_data(x0+colstart);   
  st7735_data(0x00);
  st7735_data(x1+colstart);  

  st7735_command(ST7735_RASET); 
  st7735_data(0x00);
  st7735_data(y0+rowstart);  
  st7735_data(0x00);
  st7735_data(y1+rowstart); 

  st7735_command(ST7735_RAMWR);
}

void st7735_push_colour(uint16_t colour) 
{
  SET_HIGH(st7735_config.dc_pin);
  spi_write(colour >> 8);
  spi_write(colour);
}

void st7735_draw_pixel(int16_t x, int16_t y, uint16_t colour) 
{
  if((x < 0) ||(x >= width) || (y < 0) || (y >= height)) return;

#ifdef USE_DOUBLE_BUFFERING
  uint32_t index = (x + y*SCREEN_WIDTH) * 2;
  buffer[index] = colour >> 8;
  buffer[index + 1] = colour;
#else
  st7735_set_addr_window(x, y, x+1, y+1);
  st7735_push_colour(colour);
#endif
}


void st7735_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t colour) 
{
  // Rudimentary clipping
  if((x >= width) || (y >= height)) return;
  
  if((y+h-1) >= height) {
    h = height-y;
  }

  uint8_t hi = colour >> 8, lo = colour;

#ifdef USE_DOUBLE_BUFFERING
  while (h--) {
    uint32_t index = (x + (y+h)*SCREEN_WIDTH) * 2;
    buffer[index] = hi;
    buffer[index + 1] = lo;
  }
#else
  st7735_set_addr_window(x, y, x, y+h-1);
  SET_HIGH(st7735_config.dc_pin);
  
  while (h--) {
    spi_write(hi);
    spi_write(lo);
  }
#endif

}


void st7735_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t colour) 
{
  // Rudimentary clipping
  if((x >= width) || (y >= height)) return;
  if((x+w-1) >= width)  {
    w = width-x;
  }
  uint8_t hi = colour >> 8, lo = colour;

#ifdef USE_DOUBLE_BUFFERING
  while (w--) {
    uint32_t index = ((x+w) + y*SCREEN_WIDTH) * 2;
    buffer[index] = hi;
    buffer[index + 1] = lo;
  }
#else
  st7735_set_addr_window(x, y, x+w-1, y);
  SET_HIGH(st7735_config.dc_pin);

  while (w--) {
    spi_write(hi);
    spi_write(lo);
  }
#endif
}

void st7735_fill_screen(uint16_t colour) 
{
  st7735_fill_rect(0, 0,  width, height, colour);
}

void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t colour) 
{
  if((x >= width) || (y >= height)) return;
  if((x + w - 1) >= width)  {
    w = width  - x;
  }
  if((y + h - 1) >= height) {
    h = height - y;
  }

#ifdef USE_DOUBLE_BUFFERING
   for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      st7735_draw_pixel(x, y, colour);
    }
  }
#else
  uint8_t hi = colour >> 8, lo = colour;
  st7735_set_addr_window(x, y, x+w-1, y+h-1);
  SET_HIGH(st7735_config.dc_pin);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spi_write(hi);
      spi_write(lo);
    }
  }
#endif
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed colour
uint16_t st7735_colour_565(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void st7735_set_rotation(uint8_t m) 
{
  st7735_command(ST7735_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     st7735_data(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
     width  = SCREEN_WIDTH;
     height = SCREEN_HEIGHT;
     break;
   case 1:
     st7735_data(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     width  = SCREEN_HEIGHT;
     height = SCREEN_WIDTH;
     break;
  case 2:
     st7735_data(MADCTL_BGR);
     width  = SCREEN_WIDTH;
     height = SCREEN_HEIGHT;
    break;
   case 3:
     st7735_data(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
     width  = SCREEN_HEIGHT;
     height = SCREEN_WIDTH;
     break;
  }
}

void st7735_invert_display(int8_t i) 
{
  st7735_command(i ? ST7735_INVON : ST7735_INVOFF);
}


//*****************************************************************************
//
// Graphics Functions
//
//*****************************************************************************

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour) 
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  st7735_draw_pixel(x0  , y0+r, colour);
  st7735_draw_pixel(x0  , y0-r, colour);
  st7735_draw_pixel(x0+r, y0  , colour);
  st7735_draw_pixel(x0-r, y0  , colour);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    st7735_draw_pixel(x0 + x, y0 + y, colour);
    st7735_draw_pixel(x0 - x, y0 + y, colour);
    st7735_draw_pixel(x0 + x, y0 - y, colour);
    st7735_draw_pixel(x0 - x, y0 - y, colour);
    st7735_draw_pixel(x0 + y, y0 + x, colour);
    st7735_draw_pixel(x0 - y, y0 + x, colour);
    st7735_draw_pixel(x0 + y, y0 - x, colour);
    st7735_draw_pixel(x0 - y, y0 - x, colour);
  }
}

void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t colour) 
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
      st7735_draw_pixel(x0 + x, y0 + y, colour);
      st7735_draw_pixel(x0 + y, y0 + x, colour);
    } 
    if (cornername & 0x2) {
      st7735_draw_pixel(x0 + x, y0 - y, colour);
      st7735_draw_pixel(x0 + y, y0 - x, colour);
    }
    if (cornername & 0x8) {
      st7735_draw_pixel(x0 - y, y0 + x, colour);
      st7735_draw_pixel(x0 - x, y0 + y, colour);
    }
    if (cornername & 0x1) {
      st7735_draw_pixel(x0 - y, y0 - x, colour);
      st7735_draw_pixel(x0 - x, y0 - y, colour);
    }
  }
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t colour) 
{
  st7735_draw_fast_vline(x0, y0-r, 2*r+1, colour);
  fillCircleHelper(x0, y0, r, 3, 0, colour);
}

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, 
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
      st7735_draw_fast_vline(x0+x, y0-y, 2*y+1+delta, colour);
      st7735_draw_fast_vline(x0+y, y0-x, 2*x+1+delta, colour);
    }
    if (cornername & 0x2) {
      st7735_draw_fast_vline(x0-x, y0-y, 2*y+1+delta, colour);
      st7735_draw_fast_vline(x0-y, y0-x, 2*x+1+delta, colour);
    }
  }
}

/* Bresenham's algorithm */
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour) {
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
      st7735_draw_pixel(y0, x0, colour);
    } else {
      st7735_draw_pixel(x0, y0, colour);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colour) 
{
  st7735_draw_fast_hline(x, y, w, colour);
  st7735_draw_fast_hline(x, y+h-1, w, colour);
  st7735_draw_fast_vline(x, y, h, colour);
  st7735_draw_fast_vline(x+w-1, y, h, colour);
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t colour) 
{
  st7735_draw_fast_hline(x+r  , y    , w-2*r, colour); 
  st7735_draw_fast_hline(x+r  , y+h-1, w-2*r, colour);
  st7735_draw_fast_vline(x    , y+r  , h-2*r, colour);
  st7735_draw_fast_vline(x+w-1, y+r  , h-2*r, colour);
  drawCircleHelper(x+r    , y+r    , r, 1, colour);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, colour);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, colour);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, colour);
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, 
        int16_t r, uint16_t colour) 
{
  st7735_fill_rect(x+r, y, w-2*r, h, colour);
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, colour);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, colour);
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t colour) 
{
  drawLine(x0, y0, x1, y1, colour);
  drawLine(x1, y1, x2, y2, colour);
  drawLine(x2, y2, x0, y0, colour);
}

void fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1,
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
    st7735_draw_fast_hline(a, y0, b-a+1, colour);
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
    st7735_draw_fast_hline(a, y, b-a+1, colour);
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
    st7735_draw_fast_hline(a, y, b-a+1, colour);
  }
}

/* Draw a 1-bit colour bitmap at the specified x, y position from the
 * provided bitmap buffer using colour as the foreground colour and 
 * bg as the background colour. */
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,
            uint16_t colour, uint16_t bg) 
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(*(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        st7735_draw_pixel(x+i, y+j, colour);
      }
      else {
        st7735_draw_pixel(x+i, y+j, bg);
      }
    }
  }
}

/* Draw XBitMap Files (*.xbm), exported from GIMP,
 * Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
 * C Array can be directly used with this function */
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, 
            int16_t w, int16_t h, uint16_t colour) 
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(*(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        st7735_draw_pixel(x+i, y+j, colour);
      }
    }
  }
}

void drawString(char *c, int16_t x, int16_t y, uint16_t colour, uint16_t bg, uint8_t size) {
  cursor_x = x;
  cursor_y = y;
  textsize = size;
  textcolour = colour;
  textbgcolour = bg;

  while(*c) {
    if (*c == '\n') {
      cursor_y += textsize*8;
      cursor_x  = 0;
    } else if (*c == '\r') {
      // Skip
    } else {
      drawChar(cursor_x, cursor_y, *c, textcolour, textbgcolour, textsize);
      cursor_x += textsize*6;
      if (wrap && (cursor_x > (width - textsize*6))) {
        cursor_y += textsize*8;
        cursor_x = 0;
      }
    }
    c++;
  }
}

void drawChar(int16_t x, int16_t y, unsigned char c,
          uint16_t colour, uint16_t bg, uint8_t size) {

  if((x >= width)              || 
     (y >= height)             || 
     ((x + 6 * size - 1) < 0)  || 
     ((y + 8 * size - 1) < 0)) 
    return;

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = *(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          st7735_draw_pixel(x+i, y+j, colour);
        else {  // big size
          st7735_fill_rect(x+(i*size), y+(j*size), size, size, colour);
        } 
      } else if (bg != colour) {
        if (size == 1) // default size
          st7735_draw_pixel(x+i, y+j, bg);
        else {  // big size
          st7735_fill_rect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void setCursor(int16_t x, int16_t y) 
{
  cursor_x = x;
  cursor_y = y;
}

void setTextSize(uint8_t s) 
{
  textsize = (s > 0) ? s : 1;
}

void setTextColour(uint16_t c, uint16_t b) 
{
  textcolour   = c;
  textbgcolour = b; 
}

void setTextWrap(uint16_t w) 
{
  wrap = w;
}


