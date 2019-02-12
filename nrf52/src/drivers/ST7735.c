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
#include <nrf_drv_pwm.h>
#include <app_util_platform.h>
#include <stdlib.h>
#include <string.h>
#include "ST7735.h"
#include "app/gfx_effect.h"
#include "boards.h"

// This increase the bss section by 25k !!!!
//#define USE_DOUBLE_BUFFERING

//*****************************************************************************
//
// Local Defines
//
//*****************************************************************************

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
static st7735_config_t st7735_config;

// Using a complete double buffer is a little bit too intense on the memory
// we will use a buffer that can contain 25% of the screen. 6400 bytes
#define BYTES_PER_PIXEL 2       
#define BUFFER_SIZE (ST7735_HEIGHT * ST7735_WIDTH * BYTES_PER_PIXEL)/4
static uint8_t buffer[BUFFER_SIZE] = {0};

//*****************************************************************************
//
// PWM stuff
//
//*****************************************************************************
static nrf_drv_pwm_t m_pwm2 = NRF_DRV_PWM_INSTANCE(2);

nrf_pwm_values_individual_t seq_values[1] = {0};
nrf_pwm_sequence_t const seq =
{
    .values.p_individual = seq_values,
    .length          = NRF_PWM_VALUES_LENGTH(seq_values),
    .repeats         = 0,
    .end_delay       = 0
};

static void pwm_init(void)
{

    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            st7735_config.blk_pin,                // channel 0
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
            NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 100,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    // Init PWM without error handler
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm2, &config0, NULL));
}

void st7735_set_brightness(uint8_t brightness)
{
    
    // Check if value is outside of range. If so, set to 100%
    if(brightness >= 100) {
        seq_values->channel_0 = 0;
    } else {
        seq_values->channel_0 = 100 - brightness;
    }
    
    nrf_drv_pwm_simple_playback(&m_pwm2, &seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

//*****************************************************************************
//
// SPI stuff
//
//*****************************************************************************
static nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0); // change for two with real board

static void spi_init(void) {
    nrf_drv_spi_config_t spi_config;

    spi_config.frequency = NRF_DRV_SPI_FREQ_8M;
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

static const uint8_t
    initCommands[] = {                 // Init for 7735R, (green tab)
      21,                             // 21 commands in list:
    ST7735_SWRESET,   DELAY, //  1: Software reset, 0 args, w/delay
      150,                          //     150 ms delay
    ST7735_SLPOUT ,   DELAY, //  2: Out of sleep mode, 0 args, w/delay
      255,                          //     500 ms delay
    ST7735_FRMCTR1, 3,              //  3: Framerate ctrl - normal mode, 3 arg:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3,              //  4: Framerate ctrl - idle mode, 3 args:
      0x01, 0x2C, 0x2D,             //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6,              //  5: Framerate - partial mode, 6 args:
      0x01, 0x2C, 0x2D,             //     Dot inversion mode
      0x01, 0x2C, 0x2D,             //     Line inversion mode
    ST7735_INVCTR , 1,              //  6: Display inversion ctrl, 1 arg:
      0x07,                         //     No inversion
    ST7735_PWCTR1 , 3,              //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                         //     -4.6V
      0x84,                         //     AUTO mode
    ST7735_PWCTR2 , 1,              //  8: Power control, 1 arg, no delay:
      0xC5,                         //     VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
    ST7735_PWCTR3 , 2,              //  9: Power control, 2 args, no delay:
      0x0A,                         //     Opamp current small
      0x00,                         //     Boost frequency
    ST7735_PWCTR4 , 2,              // 10: Power control, 2 args, no delay:
      0x8A,                         //     BCLK/2,
      0x2A,                         //     opamp current small & medium low
    ST7735_PWCTR5 , 2,              // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1,              // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0,              // 13: Don't invert display, no args
    ST7735_MADCTL , 1,              // 14: Mem access ctl (directions), 1 arg:
      0xC8,                         //     row/col addr, bottom-top refresh
    ST7735_COLMOD , 1,              // 15: set color mode, 1 arg, no delay:
      0x05,                       //     16-bit color                  //     100 ms delay
    ST7735_CASET  , 4,              //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x7F,                   //     XEND = 79
    ST7735_RASET  , 4,              //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,                   //     XSTART = 0
      0x00, 0x9F,                 //     XEND = 159
     ST7735_GMCTRP1, 16      ,       //  1: Gamma setting, I think?
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      ,       //  2: More gamma?
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,   DELAY, //  3: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST7735_DISPON ,   DELAY, //  4: Main screen turn on, no args w/delay
      100 };                        //     100 ms delay

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

void st7735_init(void) 
{
    if (is_init) {
        return;
    }

    st7735_config.spi = spi;
    st7735_config.sck_pin = PIN_OLED_CLK;
    st7735_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
    st7735_config.mosi_pin = PIN_OLED_DATA;
    st7735_config.cs_pin = PIN_OLED_CS;
    st7735_config.dc_pin = PIN_OLED_DC_MODE;
    st7735_config.rst_pin = PIN_OLED_RESET;
    st7735_config.blk_pin = PIN_OLED_BLK;

    spi_init();
    pwm_init();

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
    rowstart = 24;
    width = ST7735_WIDTH;
    height = ST7735_HEIGHT;
    wrap = 1;
    cursor_y = 0;
    cursor_x = 0;
    textsize = 1;
    textcolour = 0xFFFF;
    textbgcolour = 0xFFFF;

    /* Initialise LCD screen */
    command_list(initCommands);

    st7735_set_brightness(50);
    st7735_set_rotation(3);
    st7735_fill_screen(ST7735_BLACK);

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

void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
  buffer[0] = 0x00;
  buffer[2] = 0x00;

  st7735_command(ST7735_CASET); 

  SET_HIGH(st7735_config.dc_pin);
  buffer[1] = x0+colstart;
  buffer[3] = x1+colstart;
  spi_master_tx(buffer, 4);

  st7735_command(ST7735_RASET); 

  SET_HIGH(st7735_config.dc_pin);
  buffer[1] = y0+rowstart;
  buffer[3] = y1+rowstart;
  spi_master_tx(buffer, 4);

  st7735_command(ST7735_RAMWR);
}

void st7735_push_colour(uint16_t colour) 
{
  SET_HIGH(st7735_config.dc_pin);
  buffer[0] = colour >> 8;
  buffer[1] = colour;
  spi_master_tx(buffer, 2);
}

void st7735_draw_pixel(int16_t x, int16_t y, uint16_t colour) 
{
    if((x < 0) ||(x >= width) || (y < 0) || (y >= height)) {
        return;
    }

    st7735_set_addr_window(x, y, x+1, y+1);
    st7735_push_colour(colour);
}

void st7735_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t colour) 
{

    uint32_t i = 0;
    uint8_t hi, lo;

    // Rudimentary clipping
    if((x >= width) || (y >= height)) {
        return;
    }
  
    if((y+h-1) >= height) {
        h = height-y;
    }

    hi = colour >> 8;
    lo = colour;

    st7735_set_addr_window(x, y, x, y+h-1);
    SET_HIGH(st7735_config.dc_pin);
  
    while (h--) {
        buffer[i++] = hi;
        buffer[i++] = lo;
    }

    spi_master_tx(buffer, i);
}

void st7735_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t colour) 
{
    uint32_t i = 0;
    uint8_t hi, lo;

    // Rudimentary clipping
    if((x >= width) || (y >= height)) {
        return;
    }

    if((x+w-1) >= width)  {
        w = width-x;
    }

    hi = colour >> 8;
    lo = colour;

    st7735_set_addr_window(x, y, x+w-1, y);
    SET_HIGH(st7735_config.dc_pin);

    while (w--) {
        buffer[i++] = hi;
        buffer[i++] = lo;
    }

    spi_master_tx(buffer, i);
}

void st7735_fill_screen_black(void)
{
    st7735_fill_screen(ST7735_BLACK);
}

void st7735_fill_screen_white(void)
{
    st7735_fill_screen(ST7735_WHITE);
}

void st7735_fill_screen(uint16_t colour) 
{
    st7735_fill_rect(0, 0,  width, height, colour);
}

void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h,
                      uint16_t colour) 
{
    uint32_t i = 0;
    uint8_t hi, lo;

    if((x >= width) || (y >= height)) {
        return;
    }

    if((x + w - 1) >= width)  {
        w = width  - x;
    }

    if((y + h - 1) >= height) {
        h = height - y;
    }

    hi = colour >> 8;
    lo = colour;
    st7735_set_addr_window(x, y, x+w-1, y+h-1);
    SET_HIGH(st7735_config.dc_pin);

    for(y=h; y>0; y--) {
        for(x=w; x>0; x--) {
            buffer[i++] = hi;
            buffer[i++] = lo;
            // Here its possible to overflow the buffer size
            if (i == BUFFER_SIZE) {
                spi_master_tx(buffer, i);
                i = 0;
            }
        }
    }

    spi_master_tx(buffer, i);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed colour
uint16_t st7735_colour_565(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void st7735_draw_16bit_bitmap(int16_t x, int16_t y, const uint8_t *bitmap,
    int16_t w, int16_t h, uint16_t bg_color)
{
    uint16_t i, j;
    uint8_t hi, lo;
    i = j = 0;

    st7735_set_addr_window(x, y, x+w-1, y+h-1);
    SET_HIGH(st7735_config.dc_pin);

    hi = bg_color >> 8;
    lo = bg_color;

    for(y=h; y>0; y--) {
        for(x=w; x>0; x--) {
            if ((bitmap[j] == 0) && (bitmap[j+1] == 0)) {
                buffer[i++] = hi;
                buffer[i++] = lo;
                j += 2;
            } else {
                buffer[i++] = bitmap[j++];
                buffer[i++] = bitmap[j++];
            }
            // Here its possible to overflow the buffer size
            if (i == BUFFER_SIZE) {
                spi_master_tx(buffer, i);
                i = 0;
            }
        }
    }

    spi_master_tx(buffer, i);
}

void st7735_set_rotation(uint8_t m) 
{
    st7735_command(ST7735_MADCTL);
    rotation = m % 4; // can't be higher than 3
    switch (rotation) {
    case 0:
        st7735_data(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        width  = ST7735_WIDTH;
        height = ST7735_HEIGHT;
        break;
    case 1:
        st7735_data(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        width  = ST7735_HEIGHT;
        height = ST7735_WIDTH;
        break;
    case 2:
        st7735_data(MADCTL_BGR);
        width  = ST7735_WIDTH;
        height = ST7735_HEIGHT;
        break;
    case 3:
        st7735_data(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
        width  = ST7735_HEIGHT;
        height = ST7735_WIDTH;
        break;
    }

    gfx_set_rotation(m);
}

void st7735_invert_display(uint8_t i) 
{
  st7735_command(i ? ST7735_INVON : ST7735_INVOFF);
}
