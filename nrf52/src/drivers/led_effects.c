//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "led_effects.h"
#include "boards.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_pwm.h"
#include <nrf_delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void show_with_PWM(void);
void show_with_DWT(void);

// 3 leds/bytes by pixel
#define NSEC_NEOPIXEL_NUM_BYTES (NEOPIXEL_COUNT * 3)

struct Nsec_pixels {
    uint8_t brightness;
    uint8_t rOffset;
    uint8_t gOffset;
    uint8_t bOffset;
    uint8_t pixels[NSEC_NEOPIXEL_NUM_BYTES];
};

static struct Nsec_pixels nsec_pixels;

uint32_t mapConnect[] = {PIN_NEOPIXEL, NRF_PWM_PIN_NOT_CONNECTED,
                         NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

static uint32_t mapDisconnect[] = {
    NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED,
    NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

static bool initialized = false;

void nsec_neoPixel_init(void)
{
    initialized = true;

    memset(&nsec_pixels, 0, sizeof(nsec_pixels));

    // Magic number coming from Adafruit library
    nsec_pixels.rOffset = (NEO_GRB >> 4) & 0b11;
    nsec_pixels.gOffset = (NEO_GRB >> 2) & 0b11;
    nsec_pixels.bOffset = NEO_GRB & 0b11;

    // Configure pin
    nrf_gpio_cfg_output(PIN_NEOPIXEL);
    nrf_gpio_pin_clear(PIN_NEOPIXEL);
}

void nsec_neoPixel_clear(void) {
    memset(nsec_pixels.pixels, 0, sizeof(nsec_pixels.pixels));
}

// Set the n pixel color
void nsec_neoPixel_set_pixel_color(uint16_t n, uint8_t r, uint8_t g,
                                   uint8_t b) {
    if (n < NEOPIXEL_COUNT) {
        if (nsec_pixels.brightness) {
            r = (r * nsec_pixels.brightness) >> 8;
            g = (g * nsec_pixels.brightness) >> 8;
            b = (b * nsec_pixels.brightness) >> 8;
        }

        uint8_t *p;
        p = &nsec_pixels.pixels[n * 3];
        p[nsec_pixels.rOffset] = r;
        p[nsec_pixels.gOffset] = g;
        p[nsec_pixels.bOffset] = b;
    }
}

void nsec_neoPixel_set_pixel_color_packed(uint16_t n, uint32_t c) {
    if (n < NEOPIXEL_COUNT) {
        uint8_t r = (uint8_t)(c >> 16);
        uint8_t g = (uint8_t)(c >> 8);
        uint8_t b = (uint8_t)c;

        nsec_neoPixel_set_pixel_color(n, r, g, b);
    }
}

uint32_t nsec_neoPixel_get_pixel_color(uint16_t n) {
    if (n >= NEOPIXEL_COUNT)
        return 0;

    uint8_t *pixel;
    pixel = &nsec_pixels.pixels[n * 3];
    if (nsec_pixels.brightness) {
        return (((uint32_t)(pixel[nsec_pixels.rOffset] << 8) /
                 nsec_pixels.brightness)
                << 16) |
               (((uint32_t)(pixel[nsec_pixels.gOffset] << 8) /
                 nsec_pixels.brightness)
                << 8) |
               ((uint32_t)(pixel[nsec_pixels.bOffset] << 8) /
                nsec_pixels.brightness);
    } else {
        return ((uint32_t)pixel[nsec_pixels.rOffset] << 16) |
               ((uint32_t)pixel[nsec_pixels.gOffset] << 8) |
               (uint32_t)pixel[nsec_pixels.bOffset];
    }
}

void nsec_neoPixel_set_brightness(uint8_t b) {
    uint8_t newBrightness = b + 1;
    if (newBrightness != nsec_pixels.brightness) {
        uint8_t *ptr = nsec_pixels.pixels;
        uint8_t oldBrighness = nsec_pixels.brightness - 1;
        uint16_t scale;

        if (oldBrighness == 0) {
            scale = 0;
        } else if (b == 255) {
            scale = 65535;
        } else {
            scale = (((uint16_t)newBrightness << 8) - 1) / oldBrighness;
        }

        for (size_t i = 0; i < ARRAY_SIZE(nsec_pixels.pixels); i++) {
            uint8_t pixel = nsec_pixels.pixels[i];
            nsec_pixels.pixels[i] = (pixel * scale) >> 8;
        }
        nsec_pixels.brightness = newBrightness;
    }
}

uint8_t nsec_neoPixel_get_brightness(void) {
    return nsec_pixels.brightness - 1;
}

void nsec_neoPixel_show(void) {
    ASSERT(initialized);

    show_with_PWM();
    // show_with_DWT();

    nrf_delay_us(50);
}

void show_with_PWM(void) {
    // todo Implement the canshow
    uint16_t pixels_pattern[NSEC_NEOPIXEL_NUM_BYTES * 8 + 2];

    if (pixels_pattern != NULL && nsec_pixels.pixels != NULL) {
        uint16_t pos = 0;

        for (size_t n = 0; n < ARRAY_SIZE(nsec_pixels.pixels); n++) {
            uint8_t pix = nsec_pixels.pixels[n];

            for (uint8_t mask = 0x80, i = 0; mask > 0; mask >>= 1, i++) {
                pixels_pattern[pos] = (pix & mask) ? MAGIC_T1H : MAGIC_T0H;
                pos++;
            }
        }
        // Zero padding to indicate the end of que sequence
        pixels_pattern[++pos] = 0 | (0x8000);
        pixels_pattern[++pos] = 0 | (0x8000);
    } else {
        return;
    }

    nrf_pwm_configure(NRF_PWM0, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, CTOPVAL);
    nrf_pwm_loop_set(NRF_PWM0, 0);
    nrf_pwm_decoder_set(NRF_PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);

    // Configure the sequence
    nrf_pwm_seq_ptr_set(NRF_PWM0, 0, pixels_pattern);
    nrf_pwm_seq_cnt_set(NRF_PWM0, 0, ARRAY_SIZE(pixels_pattern));
    nrf_pwm_seq_refresh_set(NRF_PWM0, 0, 0);
    nrf_pwm_seq_end_delay_set(NRF_PWM0, 0, 0);
    nrf_pwm_pins_set(NRF_PWM0, mapConnect);

    // Enable the PWM
    nrf_pwm_enable(NRF_PWM0);

    // Start the sequence
    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);
    nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_SEQSTART0);

    while (!nrf_pwm_event_check(NRF_PWM0, NRF_PWM_EVENT_SEQEND0))
        ;

    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);

    // Disable the PWM
    nrf_pwm_disable(NRF_PWM0);

    nrf_pwm_pins_set(NRF_PWM0, mapDisconnect);
}

void show_with_DWT(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    while (1) {
        uint32_t cycStart = DWT->CYCCNT;
        uint32_t cycle = 0;

        for (size_t n = 0; n < ARRAY_SIZE(nsec_pixels.pixels); n++) {
            uint8_t pix = nsec_pixels.pixels[n];

            for (uint8_t mask = 0x80; mask; mask >>= 1) {
                while (DWT->CYCCNT - cycle < CYCLES_800)
                    ;

                cycle = DWT->CYCCNT;

                nrf_gpio_pin_set(PIN_NEOPIXEL);

                if (pix & mask) {
                    while (DWT->CYCCNT - cycle < CYCLES_800_T1H)
                        ;
                } else {
                    while (DWT->CYCCNT - cycle < CYCLES_800_T0H)
                        ;
                }

                nrf_gpio_pin_clear(PIN_NEOPIXEL);
            }
        }

        while (DWT->CYCCNT - cycle < CYCLES_800)
            ;

        // If total time is longer than 25%, resend the whole data.
        // Since we are likely to be interrupted by SoftDevice
        if ((DWT->CYCCNT - cycStart) <
            (8 * NSEC_NEOPIXEL_NUM_BYTES * ((CYCLES_800 * 5) / 4))) {
            break;
        }
        nrf_delay_us(300);
    }
}
