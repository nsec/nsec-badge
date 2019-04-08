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
#include "ST7735.h"
#include "app/external_flash.h"
#include "app/gfx_effect.h"
#include "bitmap.h"
#include "boards.h"
#include "flash.h"
#include <app_util_platform.h>
#include <nrf.h>
#include <nrf_delay.h>
#include <nrf_drv_pwm.h>
#include <nrf_gpio.h>
#include <stdlib.h>
#include <string.h>

// This increase the bss section by 25k !!!!
//#define USE_DOUBLE_BUFFERING

//*****************************************************************************
//
// Local Defines
//
//*****************************************************************************

#define DELAY 0x80

#define swap(a, b)                                                             \
    {                                                                          \
        int16_t t = a;                                                         \
        a = b;                                                                 \
        b = t;                                                                 \
    }

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
int16_t wrap, cursor_y, cursor_x;
uint16_t textcolour, textbgcolour;
uint32_t width, height;
static bool is_init = false;
static st7735_config_t st7735_config;

// Using a complete double buffer is a little bit too intense on the memory
// we will use a buffer that can contain 25% of the screen. 6400 bytes
#define BYTES_PER_PIXEL 2
#define BUFFER_SIZE (ST7735_HEIGHT * ST7735_WIDTH * BYTES_PER_PIXEL) / 4
static uint8_t buffer[BUFFER_SIZE] = {0};

//*****************************************************************************
//
// PWM stuff
//
//*****************************************************************************
static nrf_drv_pwm_t m_pwm2 = NRF_DRV_PWM_INSTANCE(CONF_OLED_PWM_INST);

nrf_pwm_values_individual_t seq_values[1] = {{0, 0, 0, 0}};
nrf_pwm_sequence_t const seq = {.values.p_individual = seq_values,
                                .length = NRF_PWM_VALUES_LENGTH(seq_values),
                                .repeats = 0,
                                .end_delay = 0};

static void pwm_init(void)
{

    nrf_drv_pwm_config_t const config0 = {
        .output_pins =
            {
                st7735_config.blk_pin,    // channel 0
                NRF_DRV_PWM_PIN_NOT_USED, // channel 1
                NRF_DRV_PWM_PIN_NOT_USED, // channel 2
                NRF_DRV_PWM_PIN_NOT_USED, // channel 3
            },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock = NRF_PWM_CLK_16MHz,
        .count_mode = NRF_PWM_MODE_UP,
        .top_value = 100,
        .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = NRF_PWM_STEP_AUTO};
    // Init PWM without error handler
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm2, &config0, NULL));
}

void st7735_set_brightness(uint8_t brightness)
{

    // Check if value is outside of range. If so, set to 100%
    if (brightness >= 100) {
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
static nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(CONF_OLED_SPI_INST);

static void spi_init(void)
{
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

    APP_ERROR_CHECK(
        nrf_drv_spi_init(&st7735_config.spi, &spi_config, NULL, NULL));
}


/*
 * Send bytes of data to the LCD.
 */
static void st7735_data_len(const uint8_t *p_tx_data, uint16_t len)
{
    while (len > 0) {
        const uint8_t packet_len = MIN(len, UINT8_MAX);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&st7735_config.spi, p_tx_data,
                                             packet_len, NULL, 0));
        len -= packet_len;
        p_tx_data += packet_len;
    }
}

/*
 * Send 1 byte of data to the LCD.
 */
static void st7735_data(uint8_t data)
{
    st7735_data_len(&data, 1);
}

/*
 * Send a 1 byte command to the LCD.
 */
static void st7735_command(uint8_t command)
{
    nrf_gpio_pin_write(st7735_config.dc_pin, COMMAND);
    st7735_data_len(&command, 1);
    nrf_gpio_pin_write(st7735_config.dc_pin, DATA);
}

/*
 * Software reset LCD module to default
 */
static void st7735_software_reset(void)
{
    st7735_command(ST7735_SWRESET);

    /*
     * Wait 120ms for hardware initialization
     * before sending the next command.
     */
    nrf_delay_ms(120);
}

/*
 * Hardware reset LCD module to default
 */
static void st7735_hardware_reset(void)
{
    /* Send a reset pulse */
    nrf_gpio_pin_toggle(st7735_config.rst_pin);
    nrf_delay_ms(1);
    nrf_gpio_pin_toggle(st7735_config.rst_pin);

    /*
     * Wait 120ms for hardware initialization
     * before sending the next command.
     */
    nrf_delay_ms(120);
}

/*
 * Set the framerate for all modes
 *
 * st7735r / st7735s:
 *   fosc=850kHz
 *   rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA + 2))
 *
 * st7735:
 *   fosc=333kHz
 *   rate=fosc/((RTNA + 20) x (LINE + FPA + BPA))
 */
static void st7735_set_framerate(void)
{
    uint8_t rtna = 0x01;
    uint8_t fpa = 0x2C;
    uint8_t bpa = 0x2D;

    /* Normal mode */
    st7735_command(ST7735_FRMCTR1);
    st7735_data(fpa);
    st7735_data(bpa);

    /* Idle mode */
    st7735_command(ST7735_FRMCTR2);
    st7735_data(fpa);
    st7735_data(bpa);

    /* Partial mode */
    st7735_command(ST7735_FRMCTR3);
    st7735_data(fpa);
    st7735_data(bpa);
    st7735_data(fpa);
    st7735_data(bpa);
}

/*
 * 0x07: Column inversion (default)
 * 0x00: Dot inversion
 */
static void st7735_set_inversion_mode(void)
{
    st7735_command(ST7735_INVCTR);
    st7735_data(0x07);
}

static void st7735_inversion_on(void)
{
    st7735_command(ST7735_INVON);
}

static void st7735_inversion_off(void)
{
    st7735_command(ST7735_INVOFF);
}

/*
 * Set the LCD to sleep mode
 *
 * Stop DC/DC converter, oscillator and panel scanning.
 */
static void st7735_sleep_in(void)
{
    st7735_command(ST7735_SLPIN);

    /*
     * Wait 120ms for the stabilization of
     * supply voltages.
     */
    nrf_delay_ms(120);
}

/*
 * Exit sleep mode.
 *
 * Enable DC/DC converter, oscillator and start panel scanning.
 */
static void st7735_sleep_out(void)
{
    st7735_command(ST7735_SLPOUT);

    /*
     * Wait 120ms for the stabilization of
     * supply voltages.
     */
    nrf_delay_ms(120);
}

/*
 * Set the pixel size for RGB data
 *
 * 12-bits : 0x03
 * 16-bits : 0x05
 * 18-bits : 0x06 (default)
 */
static void st7735_set_pixel_format(uint8_t fmt)
{
    st7735_command(ST7735_COLMOD);
    st7735_data(fmt);
}

static void st7735_set_gamma_pos(uint8_t vrf0p, uint8_t selv0p, uint8_t selv1p,
                                 uint8_t pk0p, uint8_t pk1p, uint8_t pk2p,
                                 uint8_t pk3p, uint8_t pk4p, uint8_t pk5p,
                                 uint8_t pk6p, uint8_t pk7p, uint8_t pk8p,
                                 uint8_t pk9p, uint8_t selv62p, uint8_t selv63p,
                                 uint8_t vos0p)
{
    st7735_command(ST7735_GMCTRP1);
    st7735_data(vrf0p);
    st7735_data(selv0p);
    st7735_data(selv1p);
    st7735_data(pk0p);
    st7735_data(pk1p);
    st7735_data(pk2p);
    st7735_data(pk3p);
    st7735_data(pk4p);
    st7735_data(pk5p);
    st7735_data(pk6p);
    st7735_data(pk7p);
    st7735_data(pk8p);
    st7735_data(pk9p);
    st7735_data(selv62p);
    st7735_data(selv63p);
    st7735_data(vos0p);
}

static void st7735_set_gamma_neg(uint8_t vrf0n, uint8_t selv0n, uint8_t selv1n,
                                 uint8_t pk0n, uint8_t pk1n, uint8_t pk2n,
                                 uint8_t pk3n, uint8_t pk4n, uint8_t pk5n,
                                 uint8_t pk6n, uint8_t pk7n, uint8_t pk8n,
                                 uint8_t pk9n, uint8_t selv62n, uint8_t selv63n,
                                 uint8_t vos0n)
{
    st7735_command(ST7735_GMCTRN1);
    st7735_data(vrf0n);
    st7735_data(selv0n);
    st7735_data(selv1n);
    st7735_data(pk0n);
    st7735_data(pk1n);
    st7735_data(pk2n);
    st7735_data(pk3n);
    st7735_data(pk4n);
    st7735_data(pk5n);
    st7735_data(pk6n);
    st7735_data(pk7n);
    st7735_data(pk8n);
    st7735_data(pk9n);
    st7735_data(selv62n);
    st7735_data(selv63n);
    st7735_data(vos0n);
}

static void st7735_set_power_1(void)
{
    st7735_command(ST7735_PWCTR1);
    st7735_data(0xA2); // AVDD: 5V GVDD: 4.6V
    st7735_data(0x02); // GVCL: -4.6V
    st7735_data(0x84); // Auto mode
}

static void st7735_set_power_2(void)
{
    st7735_command(ST7735_PWCTR2);
    st7735_data(0xC5); // VGH25=2.4C VGSEL=-10 VGH=3 * AVDD
}

/*
 * Normal mode, full color
 */
static void st7735_set_power_3(void)
{
    st7735_command(ST7735_PWCTR3);
    st7735_data(0x0A); // Opamp current small
    st7735_data(0x00); // Boost frequency
}

/*
 * Idle mode, 8 color
 */
static void st7735_set_power_4(void)
{
    st7735_command(ST7735_PWCTR4);
    st7735_data(0x8A); // BCLK/2,
    st7735_data(0x2A); // opamp current small & medium low
}

/*
 * Partial mode, full color
 */
static void st7735_set_power_5(void)
{
    st7735_command(ST7735_PWCTR5);
    st7735_data(0x8A); //
    st7735_data(0xEE);
}

static void st7735_set_vcom(void)
{
    st7735_command(ST7735_VMCTR1);
    st7735_data(0x0E); // -0.775
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

    /* Set transfer to DATA mode by default, we toggle to COMMAND only when
     * issuing a command
     */
    nrf_gpio_pin_write(st7735_config.dc_pin, DATA);

    /* Reset pin has to be held high to enable the LCD */
    nrf_gpio_pin_set(st7735_config.rst_pin);

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
    st7735_software_reset();
    st7735_sleep_out();

    /* Use 16-bits pixels */
    st7735_set_pixel_format(ST7735_PIXEL_16BITS);

    st7735_set_brightness(50);
    st7735_set_rotation(3);

    /* Initialize the framebuffer, content is random after reset */
    st7735_fill_screen(ST7735_BLACK);

    /* And finally, start displaying */
    st7735_display_on();

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

    buffer[1] = x0 + colstart;
    buffer[3] = x1 + colstart;
    st7735_data_len(buffer, 4);

    st7735_command(ST7735_RASET);

    buffer[1] = y0 + rowstart;
    buffer[3] = y1 + rowstart;
    st7735_data_len(buffer, 4);

    st7735_command(ST7735_RAMWR);
}

void st7735_push_colour(uint16_t colour)
{
    buffer[0] = colour >> 8;
    buffer[1] = colour;
    st7735_data_len(buffer, 2);
}

void st7735_draw_pixel(int16_t x, int16_t y, uint16_t colour)
{
    if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
        return;
    }

    st7735_set_addr_window(x, y, x + 1, y + 1);
    st7735_push_colour(colour);
}

void st7735_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t colour)
{

    uint32_t i = 0;
    uint8_t hi, lo;

    // Rudimentary clipping
    if ((x >= width) || (y >= height)) {
        return;
    }

    if ((y + h - 1) >= height) {
        h = height - y;
    }

    hi = colour >> 8;
    lo = colour;

    st7735_set_addr_window(x, y, x, y + h - 1);

    while (h--) {
        buffer[i++] = hi;
        buffer[i++] = lo;
    }

    st7735_data_len(buffer, i);
}

void st7735_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t colour)
{
    uint32_t i = 0;
    uint8_t hi, lo;

    // Rudimentary clipping
    if ((x >= width) || (y >= height)) {
        return;
    }

    if ((x + w - 1) >= width) {
        w = width - x;
    }

    hi = colour >> 8;
    lo = colour;

    st7735_set_addr_window(x, y, x + w - 1, y);

    while (w--) {
        buffer[i++] = hi;
        buffer[i++] = lo;
    }

    st7735_data_len(buffer, i);
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
    st7735_fill_rect(0, 0, width, height, colour);
}

void st7735_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h,
                      uint16_t colour)
{
    uint32_t i = 0;
    uint8_t hi, lo;

    if ((x >= width) || (y >= height)) {
        return;
    }

    if ((x + w - 1) >= width) {
        w = width - x;
    }

    if ((y + h - 1) >= height) {
        h = height - y;
    }

    hi = colour >> 8;
    lo = colour;
    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            buffer[i++] = hi;
            buffer[i++] = lo;
            // Here its possible to overflow the buffer size
            if (i == BUFFER_SIZE) {
                st7735_data_len(buffer, i);
                i = 0;
            }
        }
    }

    st7735_data_len(buffer, i);
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

    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);

    hi = bg_color >> 8;
    lo = bg_color;

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            if ((bitmap[j] == 0) && (bitmap[j + 1] == 0)) {
                buffer[i++] = hi;
                buffer[i++] = lo;
                j += 2;
            } else {
                buffer[i++] = bitmap[j++];
                buffer[i++] = bitmap[j++];
            }
            // Here its possible to overflow the buffer size
            if (i == BUFFER_SIZE) {
                st7735_data_len(buffer, i);
                i = 0;
            }
        }
    }

    st7735_data_len(buffer, i);
}

// Draw an image from the external flash.
// TODO: this was duplicated from st7735_draw_16bit_bitmap, maybe it could
// share some code with it.
void st7735_draw_16bit_ext_bitmap(int16_t x, int16_t y,
                                  const struct bitmap_ext *bitmap_ext,
                                  uint16_t bg_color)
{
    static uint8_t flash_buffer[128];
    uint16_t output_idx = 0;
    uint32_t w = bitmap_ext->width;
    uint32_t h = bitmap_ext->height;

    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);

    uint8_t bg_hi = bg_color >> 8;
    uint8_t bg_lo = bg_color;

    uint8_t flash_buffer_idx = 128;
    uint32_t flash_addr = bitmap_ext->flash_data->offset;

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            if (flash_buffer_idx >= 128) {
                flash_read_128(flash_addr, flash_buffer);
                flash_addr += 128;
                flash_buffer_idx = 0;
            }

            if ((flash_buffer[flash_buffer_idx] == 0) &&
                (flash_buffer[flash_buffer_idx + 1] == 0)) {
                buffer[output_idx++] = bg_hi;
                buffer[output_idx++] = bg_lo;
                flash_buffer_idx += 2;
            } else {
                buffer[output_idx++] = flash_buffer[flash_buffer_idx++];
                buffer[output_idx++] = flash_buffer[flash_buffer_idx++];
            }
            // Here its possible to overflow the buffer size
            if (output_idx == BUFFER_SIZE) {
                st7735_data_len(buffer, output_idx);
                output_idx = 0;
            }
        }
    }

    st7735_data_len(buffer, output_idx);
}

void st7735_set_rotation(uint8_t m)
{
    st7735_command(ST7735_MADCTL);
    rotation = m % 4; // can't be higher than 3
    switch (rotation) {
    case 0:
        st7735_data(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        width = ST7735_WIDTH;
        height = ST7735_HEIGHT;
        break;
    case 1:
        st7735_data(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        width = ST7735_HEIGHT;
        height = ST7735_WIDTH;
        break;
    case 2:
        st7735_data(MADCTL_BGR);
        width = ST7735_WIDTH;
        height = ST7735_HEIGHT;
        break;
    case 3:
        st7735_data(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
        width = ST7735_HEIGHT;
        height = ST7735_WIDTH;
        break;
    }

    gfx_set_rotation(m);
}

void st7735_invert_display(uint8_t i)
{
    st7735_command(i ? ST7735_INVON : ST7735_INVOFF);
}

/*
 * Turn display off.
 * framebuffer memory is not affected.
 */
void st7735_display_off(void)
{
    st7735_command(ST7735_DISPOFF);
}

/*
 * Turn display on.
 */
void st7735_display_on(void)
{
    st7735_command(ST7735_DISPON);
}

/*
 * Turn partial mode off.
 */
void st7735_partial_off(void)
{
    st7735_command(ST7735_NORON);
}

/*
 * Turn partial mode on.
 */
void st7735_partial_on(void)
{
    st7735_command(ST7735_PTLON);
}
