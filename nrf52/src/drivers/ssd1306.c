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
All text above, and the splash screen below must be included in any
redistribution
*********************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <app_error.h>
#include <app_util_platform.h>
#include <nrf52.h>
#include <nrf_delay.h>
#include <nrf_drv_spi.h>
#include <nrf_error.h>
#include <nrf_gpio.h>

#include "boards.h"

#include "ssd1306.h"

// TODO TO REMOVE WHEN GFX WILL BE DONE MOVING TO gfx_effect
#include "app/gfx_effect.h"

static uint8_t gfx_rotation = 0;
uint8_t ssd1306_vccstate = SSD1306_SWITCHCAPVCC;

typedef enum {
    COMMAND = 0,
    DATA = 1,
} t_screen_mode;

/*
 * SPI stuff
 */
static nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(CONF_OLED_SPI_INST);

static void spi_init() {
    nrf_drv_spi_config_t spi_config;

    spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
    spi_config.sck_pin = PIN_OLED_CLK;
    spi_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.mosi_pin = PIN_OLED_DATA;
    spi_config.ss_pin = PIN_OLED_CS;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.irq_priority = APP_IRQ_PRIORITY_LOW;
    spi_config.orc = 0xFF;

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
}

static void spi_master_tx(const uint8_t *p_tx_data, uint16_t len) {
    while (len > 0) {
        const uint8_t packet_len = MIN(len, UINT8_MAX);
        APP_ERROR_CHECK(
            nrf_drv_spi_transfer(&spi, p_tx_data, packet_len, NULL, 0));
        len -= packet_len;
        p_tx_data += packet_len;
    }
}

/*
 * SSD1306 stuff
 */

// the memory buffer for the LCD
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {0};

// the most basic function, set a single pixel
void ssd1306_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) ||
        (y >= SSD1306_LCDHEIGHT))
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
    switch (color) {
    case SSD1306_WHITE:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
        break;
    case SSD1306_BLACK:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
        break;
    case SSD1306_INVERSE:
        buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
        break;
    }
}

void ssd1306_init(void) {
    spi_init();

    nrf_gpio_cfg_output(PIN_OLED_RESET);
    nrf_gpio_cfg_output(PIN_OLED_DC_MODE);

    nrf_gpio_pin_write(PIN_OLED_DC_MODE, COMMAND);
    nrf_gpio_pin_write(PIN_OLED_RESET, 1);
    nrf_delay_ms(1);
    nrf_gpio_pin_write(PIN_OLED_RESET, 0);
    nrf_delay_ms(10);
    nrf_gpio_pin_write(PIN_OLED_RESET, 1);

#if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);         // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5
    ssd1306_command(0x80);                       // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);       // 0xA8
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);   // 0xD3
    ssd1306_command(0x0);                        // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);         // 0x8D
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    } else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE); // 0x20
    ssd1306_command(0x00);               // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS); // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST); // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    } else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT); // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME); // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);       // 0xA6
#endif

#if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);         // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5
    ssd1306_command(0x80);                       // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);       // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);   // 0xD3
    ssd1306_command(0x0);                        // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);         // 0x8D
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    } else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE); // 0x20
    ssd1306_command(0x00);               // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS); // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST); // 0x81
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x9F);
    } else {
        ssd1306_command(0xCF);
    }
    ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    } else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT); // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME); // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);       // 0xA6
#endif

#if defined SSD1306_96_16
    // Init sequence for 96x16 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);         // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5
    ssd1306_command(0x80);                       // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);       // 0xA8
    ssd1306_command(0x0F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);   // 0xD3
    ssd1306_command(0x00);                       // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);         // 0x8D
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    } else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE); // 0x20
    ssd1306_command(0x00);               // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);  // 0xDA
    ssd1306_command(0x2);                 // ada x12
    ssd1306_command(SSD1306_SETCONTRAST); // 0x81
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    } else {
        ssd1306_command(0xAF);
    }
    ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
    if (ssd1306_vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    } else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT); // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME); // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);       // 0xA6
#endif

    ssd1306_command(SSD1306_DISPLAYON); //--turn on oled panel

    nrf_delay_ms(1);
}

void ssd1306_invert_display(uint8_t i) {
    if (i) {
        ssd1306_command(SSD1306_INVERTDISPLAY);
    } else {
        ssd1306_command(SSD1306_NORMALDISPLAY);
    }
}

void ssd1306_command(uint8_t c) {
    nrf_gpio_pin_write(PIN_OLED_DC_MODE, COMMAND);
    spi_master_tx(&c, 1);
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_start_scroll_right(uint8_t start, uint8_t stop) {
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
void ssd1306_start_scroll_left(uint8_t start, uint8_t stop) {
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
void ssd1306_start_scroll_diag_right(uint8_t start, uint8_t stop) {
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
void ssd1306_start_scroll_diag_left(uint8_t start, uint8_t stop) {
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

void ssd1306_stop_scroll(void) { ssd1306_command(SSD1306_DEACTIVATE_SCROLL); }

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
    ssd1306_command(0);                    // Column start address (0 = reset)
    ssd1306_command(SSD1306_LCDWIDTH - 1); // Column end address (127 = reset)

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

    nrf_gpio_pin_write(PIN_OLED_DC_MODE, DATA);
    spi_master_tx(buffer, SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8);
}

// clear everything
void ssd1306_clear_display(void) {
    memset(buffer, 0, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
}

void ssd1306_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    bool bSwap = false;
    switch (gfx_rotation) {
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
        x -= (w - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y  and
        // adjust y for w (not to become h)
        bSwap = true;
        swap(x, y);
        y = SSD1306_LCDHEIGHT - y - 1;
        y -= (w - 1);
        break;
    }

    if (bSwap) {
        ssd1306_draw_fast_vline_internal(x, y, w, color);
    } else {
        ssd1306_draw_fast_hline_internal(x, y, w, color);
    }
}

void ssd1306_draw_fast_hline_internal(int16_t x, int16_t y, int16_t w,
                                      uint16_t color) {
    // Do bounds/limit checks
    if (y < 0 || y >= SSD1306_LCDHEIGHT) {
        return;
    }

    // make sure we don't try to draw below 0
    if (x < 0) {
        w += x;
        x = 0;
    }

    // make sure we don't go off the edge of the display
    if ((x + w) > SSD1306_LCDWIDTH) {
        w = (SSD1306_LCDWIDTH - x);
    }

    // if our width is now negative, punt
    if (w <= 0) {
        return;
    }

    // set up the pointer for  movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    register uint8_t mask = 1 << (y & 7);

    switch (color) {
    case SSD1306_WHITE:
        while (w--) {
            *pBuf++ |= mask;
        };
        break;
    case SSD1306_BLACK:
        mask = ~mask;
        while (w--) {
            *pBuf++ &= mask;
        };
        break;
    case SSD1306_INVERSE:
        while (w--) {
            *pBuf++ ^= mask;
        };
        break;
    }
}

void ssd1306_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color) {
    bool bSwap = false;
    switch (gfx_rotation) {
    case 0:
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x and adjust
        // x for h (now to become w)
        bSwap = true;
        swap(x, y);
        x = SSD1306_LCDWIDTH - x - 1;
        x -= (h - 1);
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for height.
        x = SSD1306_LCDWIDTH - x - 1;
        y = SSD1306_LCDHEIGHT - y - 1;
        y -= (h - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y
        bSwap = true;
        swap(x, y);
        y = SSD1306_LCDHEIGHT - y - 1;
        break;
    }

    if (bSwap) {
        ssd1306_draw_fast_hline_internal(x, y, h, color);
    } else {
        ssd1306_draw_fast_vline_internal(x, y, h, color);
    }
}

void ssd1306_draw_fast_vline_internal(int16_t x, int16_t __y, int16_t __h,
                                      uint16_t color) {

    // do nothing if we're off the left or right side of the screen
    if (x < 0 || x >= SSD1306_LCDWIDTH) {
        return;
    }

    // make sure we don't try to draw below 0
    if (__y < 0) {
        // __y is negative, this will subtract enough from __h to account for
        // __y being 0
        __h += __y;
        __y = 0;
    }

    // make sure we don't go past the height of the display
    if ((__y + __h) > SSD1306_LCDHEIGHT) {
        __h = (SSD1306_LCDHEIGHT - __y);
    }

    // if our height is now negative, punt
    if (__h <= 0) {
        return;
    }

    // this display doesn't need ints for coordinates, use local byte registers
    // for faster juggling
    register uint8_t y = __y;
    register uint8_t h = __h;

    // set up the pointer for fast movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    // do the first partial byte, if necessary - this requires some masking
    register uint8_t mod = (y & 7);
    if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;

        // note - lookup table results in a nearly 10% performance improvement
        // in fill* functions register uint8_t mask = ~(0xFF >> (mod));
        static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0,
                                     0xF0, 0xF8, 0xFC, 0xFE};
        register uint8_t mask = premask[mod];

        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod) {
            mask &= (0XFF >> (mod - h));
        }

        switch (color) {
        case SSD1306_WHITE:
            *pBuf |= mask;
            break;
        case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
        case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
        }

        // fast exit if we're done here!
        if (h < mod) {
            return;
        }

        h -= mod;

        pBuf += SSD1306_LCDWIDTH;
    }

    // write solid bytes while we can - effectively doing 8 rows at a time
    if (h >= 8) {
        if (color ==
            SSD1306_INVERSE) { // separate copy of the code so we don't impact
                               // performance of the black/white write version
                               // with an extra comparison per loop
            do {
                *pBuf = ~(*pBuf);

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to do
                // this, but this should still help a fair bit for now)
                h -= 8;
            } while (h >= 8);
        } else {
            // store a local value to work with
            register uint8_t val = (color == SSD1306_WHITE) ? 255 : 0;

            do {
                // write our value in
                *pBuf = val;

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to do
                // this, but this should still help a fair bit for now)
                h -= 8;
            } while (h >= 8);
        }
    }

    // now do the final partial byte, if necessary
    if (h) {
        mod = h & 7;
        // this time we want to mask the low bits of the byte, vs the high bits
        // we did above register uint8_t mask = (1 << mod) - 1; note - lookup
        // table results in a nearly 10% performance improvement in fill*
        // functions
        static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07,
                                      0x0F, 0x1F, 0x3F, 0x7F};
        register uint8_t mask = postmask[mod];
        switch (color) {
        case SSD1306_WHITE:
            *pBuf |= mask;
            break;
        case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
        case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
        }
    }
}

void ssd1306_fill_screen_black(void) { memset(buffer, 0x00, sizeof(buffer)); }

void ssd1306_fill_screen_white(void) { memset(buffer, 0xFF, sizeof(buffer)); }
