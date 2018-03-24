//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "led_effects.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_pwm.h"
#include <nrf_delay.h>

void show_with_PWM(void);
void show_with_DWT(void);

struct Nsec18_pixels {
    uint16_t numBytes;
    uint8_t brightness;
    uint8_t rOffset;
    uint8_t gOffset;
    uint8_t bOffset;
    uint8_t *pixels;
};

struct Nsec18_pixels *nsec18_pixels;

uint32_t mapConnect[] = {LED_PIN, NRF_PWM_PIN_NOT_CONNECTED,
                    NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

uint32_t mapDisconnect[] = {NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED,
                    NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

void nsec_neopixel_init() {
    nsec18_pixels = malloc(sizeof(struct Nsec18_pixels));

    nsec18_pixels->brightness = 0;

    //Magic number comming from Adafruit library
    nsec18_pixels->rOffset = (NEO_GRB >> 4) & 0b11;
    nsec18_pixels->gOffset = (NEO_GRB >> 2) & 0b11;
    nsec18_pixels->bOffset = NEO_GRB & 0b11;

    //Allocate three bytes for each pixels (3 led by pixel)
    nsec18_pixels->numBytes = NEOPIXEL_COUNT * 3;
    nsec18_pixels->pixels = (uint8_t *)malloc(nsec18_pixels->numBytes);

    memset(nsec18_pixels->pixels, 0, nsec18_pixels->numBytes);

    //Configure pin
    nrf_gpio_cfg_output(LED_PIN);
    nrf_gpio_pin_clear(LED_PIN);

    return;
}

void nsec_neoPixel_clean(void) {
    memset(nsec18_pixels->pixels, 0, nsec18_pixels->numBytes);
}

//Set the n pixel color
void nsec_set_pixel_color(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
    if (nsec18_pixels->brightness) {
        r = (r * nsec18_pixels->brightness) >> 8;
        g = (g * nsec18_pixels->brightness) >> 8;
        b = (b * nsec18_pixels->brightness) >> 8;
    }

    if (n < NEOPIXEL_COUNT) {
        uint8_t *p;
        p = &nsec18_pixels->pixels[n * 3];
        p[nsec18_pixels->rOffset] = r;
        p[nsec18_pixels->gOffset] = g;
        p[nsec18_pixels->bOffset] = b;
    }

}

void nsec_neopixel_set_brightness(uint8_t b)
{
    uint8_t newBrightness = b + 1;
    if (newBrightness != nsec18_pixels->brightness) {
        uint8_t c;
        uint8_t *ptr = nsec18_pixels->pixels;
        uint8_t oldBrighness = nsec18_pixels->brightness - 1;
        uint16_t scale;

        if (oldBrighness == 0) {
            scale = 0;
        } else if (b == 255) {
            scale = 65535;
        } else {
            scale = (((uint16_t)newBrightness << 8) - 1) / oldBrighness;
        }

        for (uint16_t i=0; i < nsec18_pixels->numBytes; i++) {
            c = *ptr;
            *ptr++ = (c * scale) >> 8;
        }
        nsec18_pixels->brightness = newBrightness;
    }
}

uint8_t nsec_neopixel_get_brightness(void)
{
    return nsec18_pixels->brightness - 1;
}

void nsec_neopixel_show(void) {
    if (nsec18_pixels == NULL) {
        return;
    }

    //TODO: This is suppose to be better with PWM
    // for now, DWT will be enough.
    //show_with_PWM();
    show_with_DWT();

    nrf_delay_us(50);
}

void show_with_PWM(void) {
    //todo Implement the canshow
    uint32_t pattern_size = nsec18_pixels->numBytes*8*sizeof(uint16_t)+2*sizeof(uint16_t);
    uint16_t *pixels_pattern = NULL;

    uint16_t pos = 0;
    pixels_pattern = (uint16_t *)malloc(pattern_size);
    if (pixels_pattern != NULL && nsec18_pixels->pixels != NULL) {

        for (uint16_t n=0; n < NEOPIXEL_COUNT; n++) {
            uint8_t pix = nsec18_pixels->pixels[n];

            for (uint8_t mask=0x80, i=0; mask > 0; mask >>= 1, i++) {
                pixels_pattern[pos] = (pix & mask) ? MAGIC_T1H : MAGIC_T0H;
            }
            pos++;
        }
    } else {
        return;
    }

    // Zero padding to indicate the end of que sequence
    pixels_pattern[++pos] = 0 | (0x8000);
    pixels_pattern[++pos] = 0 | (0x8000);

    nrf_pwm_configure(NRF_PWM0, NRF_PWM_CLK_16MHz, NRF_PWM_MODE_UP, CTOPVAL);
    nrf_pwm_loop_set(NRF_PWM0, 0);
    nrf_pwm_decoder_set(NRF_PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);

    //Configure the sequence
    nrf_pwm_seq_ptr_set(NRF_PWM0, 0, pixels_pattern);
    nrf_pwm_seq_cnt_set(NRF_PWM0, 0, pattern_size/sizeof(uint16_t));
    nrf_pwm_seq_refresh_set(NRF_PWM0, 0, 0);
    nrf_pwm_seq_end_delay_set(NRF_PWM0, 0, 0);
    nrf_pwm_pins_set(NRF_PWM0, mapConnect);

    // Enable the PWM
    nrf_pwm_enable(NRF_PWM0);

    // Start the sequence
    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);
    nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_SEQSTART0);

    while (!nrf_pwm_event_check(NRF_PWM0, NRF_PWM_EVENT_SEQEND0));

    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);

    //Disable the PWM
    nrf_pwm_disable(NRF_PWM0);

    nrf_pwm_pins_set(NRF_PWM0, mapDisconnect);

    free(pixels_pattern);

}

void show_with_DWT(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    while (1) {
        uint8_t *p = nsec18_pixels->pixels;
        uint32_t cycStart = DWT->CYCCNT;
        uint32_t cycle = 0;

        for (uint16_t n=0; n < nsec18_pixels->numBytes; n++) {
            uint8_t pix = *p++;

            for (uint8_t mask = 0x80; mask; mask >>=1) {
                while (DWT->CYCCNT - cycle < CYCLES_800);

                cycle = DWT->CYCCNT;

                nrf_gpio_pin_set(LED_PIN);

                if (pix & mask) {
                    while(DWT->CYCCNT - cycle < CYCLES_800_T1H);
                } else {
                    while(DWT->CYCCNT - cycle < CYCLES_800_T0H);
                }

                nrf_gpio_pin_clear(LED_PIN);
            }
        }

        while(DWT->CYCCNT - cycle < CYCLES_800);

        // If total time is longer than 25%, resend the whole data.
        // Since we are likely to be interrupted by SoftDevice
        if ((DWT->CYCCNT - cycStart) < (8*nsec18_pixels->numBytes*((CYCLES_800*5)/4))) {
            break;
        }

        nrf_delay_us(300);
    }
}