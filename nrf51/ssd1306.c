/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#include <stdint.h>
#include <string.h>

#include <app_error.h>
#include <app_util_platform.h>
#include <spi_master.h>
#include <nrf51.h>
#include <nrf_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>

#include "boards.h"
#include "glcdfont.h"

#include "ssd1306.h"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

static uint8_t gfx_rotation = 2;
uint8_t ssd1306_vccstate = SSD1306_SWITCHCAPVCC;

typedef enum {
    COMMAND = 0,
    DATA = 1,
} t_screen_mode;


/*
 * SPI stuff
 */
static volatile bool m_transfer_completed = false;

void spi_master_0_event_handler(spi_master_evt_t spi_master_evt) {
    switch (spi_master_evt.evt_type) {
        case SPI_MASTER_EVT_TRANSFER_COMPLETED:
            m_transfer_completed = true;
            break;
        default:
            // Do nothing.
            break;
    }
}

void spi_init() {
    uint32_t err_code;

    //Configure SPI master.
    spi_master_config_t spi_config = SPI_MASTER_INIT_DEFAULT;
    spi_config.SPI_Freq = SPI_FREQUENCY_FREQUENCY_M1;
    spi_config.SPI_Pin_SCK = OLED_CLK;
    spi_config.SPI_Pin_MISO = SPI_PIN_DISCONNECTED;
    spi_config.SPI_Pin_MOSI = OLED_DATA;
    spi_config.SPI_Pin_SS = OLED_CS;
    spi_config.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_MsbFirst;
    spi_config.SPI_PriorityIRQ = APP_IRQ_PRIORITY_HIGH;
    err_code = spi_master_open(SPI_MASTER_0, &spi_config);
    APP_ERROR_CHECK(err_code);
    spi_master_evt_handler_reg(SPI_MASTER_0, spi_master_0_event_handler);
}

uint8_t spi_master_tx(uint8_t * const p_tx_data, const uint16_t len) {
    m_transfer_completed = false;

    uint32_t err_code = spi_master_send_recv(SPI_MASTER_0, p_tx_data, len, NULL, 0);
    APP_ERROR_CHECK(err_code);

    while(1) {
        if (m_transfer_completed) {
            m_transfer_completed = false;
            break;
        }
    }

    return 1;
}

/*
 * SSD1306 stuff
 */

// the memory buffer for the LCD
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {0};

// the most basic function, set a single pixel
void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
        return;

    // check rotation, move pixel around if necessary
    switch (gfx_rotation) {
        case 1:
            swap(x, y);
            x = SSD1306_LCDWIDTH - x - 1;
            break;
        case 2:
            x = SSD1306_LCDWIDTH - x - 1;
            y = SSD1306_LCDHEIGHT - y - 1;
            break;
        case 3:
            swap(x, y);
            y = SSD1306_LCDHEIGHT - y - 1;
            break;
    }

    // x is which column
    switch (color)
    {
        case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
        case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
        case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
    }
}

void ssd1306_init(void) {
    spi_init();

    nrf_gpio_cfg_output(OLED_RESET);
    nrf_gpio_cfg_output(OLED_DC_MODE);

    nrf_gpio_pin_write(OLED_DC_MODE, COMMAND);
    nrf_gpio_pin_write(OLED_RESET, 1);
    nrf_delay_ms(1);
    nrf_gpio_pin_write(OLED_RESET, 0);
    nrf_delay_ms(10);
    nrf_gpio_pin_write(OLED_RESET, 1);

    #if defined SSD1306_128_32
        // Init sequence for 128x32 OLED module
        ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
        ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
        ssd1306_command(0x80);                                  // the suggested ratio 0x80
        ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
        ssd1306_command(0x1F);
        ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
        ssd1306_command(0x0);                                   // no offset
        ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
        ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x10); }
        else
        { ssd1306_command(0x14); }
        ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
        ssd1306_command(0x00);                                  // 0x0 act like ks0108
        ssd1306_command(SSD1306_SEGREMAP | 0x1);
        ssd1306_command(SSD1306_COMSCANDEC);
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x02);
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        ssd1306_command(0x8F);
        ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x22); }
        else
        { ssd1306_command(0xF1); }
        ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
        ssd1306_command(0x40);
        ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
        ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    #endif

    #if defined SSD1306_128_64
        // Init sequence for 128x64 OLED module
        ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
        ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
        ssd1306_command(0x80);                                  // the suggested ratio 0x80
        ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
        ssd1306_command(0x3F);
        ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
        ssd1306_command(0x0);                                   // no offset
        ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
        ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x10); }
        else
        { ssd1306_command(0x14); }
        ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
        ssd1306_command(0x00);                                  // 0x0 act like ks0108
        ssd1306_command(SSD1306_SEGREMAP | 0x1);
        ssd1306_command(SSD1306_COMSCANDEC);
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x12);
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x9F); }
        else
        { ssd1306_command(0xCF); }
        ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x22); }
        else
        { ssd1306_command(0xF1); }
        ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
        ssd1306_command(0x40);
        ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
        ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    #endif

    #if defined SSD1306_96_16
        // Init sequence for 96x16 OLED module
        ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
        ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
        ssd1306_command(0x80);                                  // the suggested ratio 0x80
        ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
        ssd1306_command(0x0F);
        ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
        ssd1306_command(0x00);                                   // no offset
        ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
        ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x10); }
        else
        { ssd1306_command(0x14); }
        ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
        ssd1306_command(0x00);                                  // 0x0 act like ks0108
        ssd1306_command(SSD1306_SEGREMAP | 0x1);
        ssd1306_command(SSD1306_COMSCANDEC);
        ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
        ssd1306_command(0x2);	//ada x12
        ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x10); }
        else
        { ssd1306_command(0xAF); }
        ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
        if (ssd1306_vccstate == SSD1306_EXTERNALVCC)
        { ssd1306_command(0x22); }
        else
        { ssd1306_command(0xF1); }
        ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
        ssd1306_command(0x40);
        ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
        ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    #endif

    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

    nrf_delay_ms(1);
}


void ssd1306_invertDisplay(uint8_t i) {
    if (i) {
        ssd1306_command(SSD1306_INVERTDISPLAY);
    } else {
        ssd1306_command(SSD1306_NORMALDISPLAY);
    }
}

void ssd1306_command(uint8_t c) {
    nrf_gpio_pin_write(OLED_DC_MODE, COMMAND);
    spi_master_tx(&c, 1);
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_startscrollright(uint8_t start, uint8_t stop){
    ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X00);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_startscrollleft(uint8_t start, uint8_t stop){
    ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X00);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_startscrolldiagright(uint8_t start, uint8_t stop){
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop){
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void ssd1306_stopscroll(void){
    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void ssd1306_dim(bool dim) {
  uint8_t contrast;

  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void ssd1306_update(void) {
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);   // Column start address (0 = reset)
    ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0); // Page start address (0 = reset)
    #if SSD1306_LCDHEIGHT == 64
        ssd1306_command(7); // Page end address
    #endif
    #if SSD1306_LCDHEIGHT == 32
        ssd1306_command(3); // Page end address
    #endif
    #if SSD1306_LCDHEIGHT == 16
        ssd1306_command(1); // Page end address
    #endif

    nrf_gpio_pin_write(OLED_DC_MODE, DATA);
    spi_master_tx(buffer, SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8);
}

// clear everything
void ssd1306_clearDisplay(void) {
    memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}

void ssd1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  bool bSwap = false;
  switch(gfx_rotation) {
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      swap(x, y);
      x = SSD1306_LCDWIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = SSD1306_LCDWIDTH - x - 1;
      y = SSD1306_LCDHEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      swap(x, y);
      y = SSD1306_LCDHEIGHT - y - 1;
      y -= (w-1);
      break;
  }

  if(bSwap) {
    ssd1306_drawFastVLineInternal(x, y, w, color);
  } else {
    ssd1306_drawFastHLineInternal(x, y, w, color);
  }
}

void ssd1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Do bounds/limit checks
  if(y < 0 || y >= SSD1306_LCDHEIGHT) { return; }

  // make sure we don't try to draw below 0
  if(x < 0) {
    w += x;
    x = 0;
  }

  // make sure we don't go off the edge of the display
  if( (x + w) > SSD1306_LCDWIDTH) {
    w = (SSD1306_LCDWIDTH - x);
  }

  // if our width is now negative, punt
  if(w <= 0) { return; }

  // set up the pointer for  movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  register uint8_t mask = 1 << (y&7);

  switch (color)
  {
  case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}

void ssd1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(gfx_rotation) {
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      swap(x, y);
      x = SSD1306_LCDWIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = SSD1306_LCDWIDTH - x - 1;
      y = SSD1306_LCDHEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y
      bSwap = true;
      swap(x, y);
      y = SSD1306_LCDHEIGHT - y - 1;
      break;
  }

  if(bSwap) {
    ssd1306_drawFastHLineInternal(x, y, h, color);
  } else {
    ssd1306_drawFastVLineInternal(x, y, h, color);
  }
}


void ssd1306_drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= SSD1306_LCDWIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0) {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the height of the display
  if( (__y + __h) > SSD1306_LCDHEIGHT) {
    __h = (SSD1306_LCDHEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;


  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1306_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod) {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SSD1306_LCDWIDTH;
  }


  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8) {
    if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do  {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      register uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1306_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

  // now do the final partial byte, if necessary
  if(h) {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register uint8_t mask = postmask[mod];
    switch (color)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}


static int16_t gfx_width = SSD1306_LCDWIDTH;
static int16_t gfx_height = SSD1306_LCDHEIGHT;
static int16_t gfx_cursor_y = 0;
static int16_t gfx_cursor_x = 0;
static uint8_t gfx_textsize = 1;
static uint16_t gfx_textcolor = 0xFFFF;
static uint16_t gfx_textbgcolor = 0xFFFF;
static bool gfx_wrap = true;

// Draw a circle outline
void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  ssd1306_drawPixel(x0  , y0+r, color);
  ssd1306_drawPixel(x0  , y0-r, color);
  ssd1306_drawPixel(x0+r, y0  , color);
  ssd1306_drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    ssd1306_drawPixel(x0 + x, y0 + y, color);
    ssd1306_drawPixel(x0 - x, y0 + y, color);
    ssd1306_drawPixel(x0 + x, y0 - y, color);
    ssd1306_drawPixel(x0 - x, y0 - y, color);
    ssd1306_drawPixel(x0 + y, y0 + x, color);
    ssd1306_drawPixel(x0 - y, y0 + x, color);
    ssd1306_drawPixel(x0 + y, y0 - x, color);
    ssd1306_drawPixel(x0 - y, y0 - x, color);
  }
}

void gfx_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
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
      ssd1306_drawPixel(x0 + x, y0 + y, color);
      ssd1306_drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      ssd1306_drawPixel(x0 + x, y0 - y, color);
      ssd1306_drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      ssd1306_drawPixel(x0 - y, y0 + x, color);
      ssd1306_drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      ssd1306_drawPixel(x0 - y, y0 - x, color);
      ssd1306_drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  gfx_drawFastVLine(x0, y0-r, 2*r+1, color);
  gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
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
      gfx_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      gfx_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      gfx_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      gfx_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

// Bresenham's algorithm - thx wikpedia
void gfx_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
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
      ssd1306_drawPixel(y0, x0, color);
    } else {
      ssd1306_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void gfx_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    gfx_drawFastHLine(x, y, w, color);
    gfx_drawFastHLine(x, y+h-1, w, color);
    gfx_drawFastVLine(x, y, h, color);
    gfx_drawFastVLine(x+w-1, y, h, color);
}

void gfx_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    gfx_drawLine(x, y, x, y+h-1, color);
}

void gfx_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    gfx_drawLine(x, y, x+w-1, y, color);
}

void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    for (int16_t i=x; i<x+w; i++) {
        gfx_drawFastVLine(i, y, h, color);
    }
}

void gfx_fillScreen(uint16_t color) {
    if (color == BLACK) {
        memset(buffer, 0, sizeof(buffer));
    }
    else if(color == WHITE) {
        memset(buffer, 0xFF, sizeof(buffer));
    }
    else {
        gfx_fillRect(0, 0, gfx_width, gfx_height, color);
    }
}

// Draw a rounded rectangle
void gfx_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  gfx_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  gfx_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  gfx_drawFastVLine(x    , y+r  , h-2*r, color); // Left
  gfx_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  gfx_drawCircleHelper(x+r    , y+r    , r, 1, color);
  gfx_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  gfx_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  gfx_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void gfx_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  gfx_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  gfx_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  gfx_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void gfx_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  gfx_drawLine(x0, y0, x1, y1, color);
  gfx_drawLine(x1, y1, x2, y2, color);
  gfx_drawLine(x2, y2, x0, y0, color);
}

void gfx_fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
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
    gfx_drawFastHLine(a, y0, b-a+1, color);
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
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    gfx_drawFastHLine(a, y, b-a+1, color);
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
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    gfx_drawFastHLine(a, y, b-a+1, color);
  }
}

void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        ssd1306_drawPixel(x+i, y+j, color);
      }
    }
  }
}

// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
void gfx_drawBitmapBg(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        ssd1306_drawPixel(x+i, y+j, color);
      }
      else {
        ssd1306_drawPixel(x+i, y+j, bg);
      }
    }
  }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void gfx_drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        ssd1306_drawPixel(x+i, y+j, color);
      }
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
    gfx_drawChar(gfx_cursor_x, gfx_cursor_y, c, gfx_textcolor, gfx_textbgcolor, gfx_textsize);
    gfx_cursor_x += gfx_textsize*6;
    if (gfx_wrap && (gfx_cursor_x > (gfx_width - gfx_textsize*6))) {
      gfx_cursor_y += gfx_textsize*8;
      gfx_cursor_x = 0;
    }
  }
}

// Draw a character
void gfx_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
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
      line = pgm_read_byte(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1){ // default size
          ssd1306_drawPixel(x+i, y+j, color);
        }
        else {  // big size
          gfx_fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          ssd1306_drawPixel(x+i, y+j, bg);
        else {  // big size
          gfx_fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void gfx_setCursor(int16_t x, int16_t y) {
    gfx_cursor_x = x;
    gfx_cursor_y = y;
}

void gfx_setTextSize(uint8_t s) {
    gfx_textsize = (s > 0) ? s : 1;
}

void gfx_setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    gfx_textcolor = gfx_textbgcolor = c;
}

void gfx_setTextBackgroundColor(uint16_t c, uint16_t b) {
    gfx_textcolor   = c;
    gfx_textbgcolor = b;
}

void gfx_setTextWrap(bool w) {
    gfx_wrap = w;
}

void gfx_putc(char c) {
    gfx_write((uint8_t)c);
}

void gfx_puts(char *s) {
    while (*s) {
        gfx_write((uint8_t)*s++);
    }
}

void gfx_update() {
    ssd1306_update();
}
