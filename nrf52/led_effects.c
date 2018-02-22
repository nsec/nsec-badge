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

struct nsec18_pixels* nsec18_pixels;

int nsec_neopixel_init()
{
    nsec18_pixels = malloc(sizeof(struct nsec18_pixels));
    if (nsec18_pixels == NULL) {
        return -1;
    }

    //Magic number comming from Adafruit library
    nsec18_pixels->rOffset = (NEO_GRB >> 4) & 0b11;
    nsec18_pixels->gOffset = (NEO_GRB >> 2) & 0b11;
    nsec18_pixels->bOffset = NEO_GRB & 0b11;

    //Allocate three bytes for each pixels (3 led by pixel)
    nsec18_pixels->numBytes = NEOPIXEL_COUNT * 3;
    nsec18_pixels->pixels = malloc(sizeof(nsec18_pixels->numBytes));
    if (nsec18_pixels->pixels == NULL) {
        free(nsec18_pixels);
        return -1;
    }
    memset(nsec18_pixels->pixels, 0, nsec18_pixels->numBytes);

    //Configure pin
    nrf_gpio_cfg_output(LED_PIN);
    nrf_gpio_pin_clear(LED_PIN);

    //Configure PWM
    uint32_t map[] = {LED_PIN, NRF_PWM_PIN_NOT_CONNECTED,
                    NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};
    nrf_pwm_pins_set(NRF_PWM0, map);
    nrf_pwm_configure(NRF_PWM0, PWM_PRESCALER_PRESCALER_DIV_1, NRF_PWM_MODE_UP, CTOPVAL);
    nrf_pwm_loop_set(NRF_PWM0, 0);
    nrf_pwm_decoder_set(NRF_PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);

    return 0;
}

void nsec_neoPixel_clean(void)
{

}

//Set the n pixel color
void nsec_set_pixel_color(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    //TODO add brightness
    if (n < NEOPIXEL_COUNT) {
        uint8_t *p;
        p = &nsec18_pixels->pixels[n * 3];
        p[nsec18_pixels->rOffset] = r;
        p[nsec18_pixels->gOffset] = g;
        p[nsec18_pixels->bOffset] = b;
    }

}

void nsec_neopixel_show(void)
{
    if (nsec18_pixels == NULL) {
        return;
    }

    //todo Implement the canshow
    uint32_t pattern_size = nsec18_pixels->numBytes*8*sizeof(uint16_t)+2*sizeof(uint16_t);
    uint16_t* pixels_pattern = NULL;

    uint16_t pos = 0;
    pixels_pattern = (uint16_t *) malloc(pattern_size);
    if (pixels_pattern != NULL && nsec18_pixels->pixels != NULL) {

        for (uint16_t n=0; n < NEOPIXEL_COUNT; n++) {
            uint8_t pix = nsec18_pixels->pixels[n];

            for (uint8_t mask=0x80, i=0; mask > 0; mask >>= 1, i++) {
                pixels_pattern[pos] = (pix * mask) ? MAGIC_T1H : MAGIC_T0H;
            }
            pos++;
        }
    } else {
        return;
    }

     // Zero padding to indicate the end of que sequence
    pixels_pattern[++pos] = 0 | (0x8000);
    pixels_pattern[++pos] = 0 | (0x8000);

    //Configure the sequence
    nrf_pwm_seq_ptr_set(NRF_PWM0, 0, pixels_pattern);
    nrf_pwm_seq_cnt_set(NRF_PWM0, 0, pattern_size/sizeof(uint16_t));
    nrf_pwm_seq_refresh_set(NRF_PWM0, 0, 0);
    nrf_pwm_seq_end_delay_set(NRF_PWM0, 0, 0);

    // Enable the PWM
    nrf_pwm_enable(NRF_PWM0);

    // Start the sequence
    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);
    nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_SEQSTART0);

    while (!nrf_pwm_event_check(NRF_PWM0, NRF_PWM_EVENT_SEQEND0));

    nrf_pwm_event_clear(NRF_PWM0, NRF_PWM_EVENT_SEQEND0);

    //Disable the PWM
    nrf_pwm_disable(NRF_PWM0);

    free(pixels_pattern);

    //TODO add latch
}