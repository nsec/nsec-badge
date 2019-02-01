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

  Ported to tm4c123gh6pm by Julian Fell.
 ****************************************************/

#ifndef _ADAFRUIT_ST7735H_
#define _ADAFRUIT_ST7735H_

#include <nrf_drv_spi.h>

//*****************************************************************************
//
// Screen commands
//
//*****************************************************************************

#define ST7735_WIDTH 80
#define ST7735_HEIGHT 160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

//*****************************************************************************
//
// Colour Definitions
//
//*****************************************************************************
#define ST7735_BLACK      0x0000
#define ST7735_RED        0xF800
#define ST7735_GREEN      0x07E0
#define ST7735_BLUE       0x001F
#define ST7735_CYAN       0x07FF
#define ST7735_MAGENTA    0xF81F
#define ST7735_YELLOW     0xFFE0
#define ST7735_ORANGE     0xFC00
#define ST7735_WHITE      0xFFFF

//*****************************************************************************
//
// structure Definitions
//
//*****************************************************************************

typedef struct
{
    nrf_drv_spi_t   spi; // SPI instance to use
    uint8_t         sck_pin;
    uint8_t         miso_pin;
    uint8_t         mosi_pin;
    uint8_t         cs_pin;
    uint8_t         dc_pin;
    uint8_t         rst_pin;
    uint8_t         blk_pin;
} st7735_config_t;

//*****************************************************************************
//
// Function Prototypes
//
//*****************************************************************************

void st7735_init(void);
void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void st7735_push_color(uint16_t color);
void st7735_draw_pixel(int16_t x, int16_t y, uint16_t color);
void st7735_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color);
void st7735_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);
void st7735_fill_screen(uint16_t color);
void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void st7735_fill_screen_black(void);
void st7735_fill_screen_white(void);
void st7735_set_rotation(uint8_t m);
void st7735_invert_display(uint8_t i);
void st7735_set_brightness(uint8_t brightness);
uint16_t st7735_color_565(uint8_t r, uint8_t g, uint8_t b);

#endif
