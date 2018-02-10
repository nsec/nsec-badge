//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "led_effects.h"

#include <app_timer.h>
#include <nrf_gpio.h>
#include "boards.h"

APP_TIMER_DEF(led_timer);

static const uint8_t led_pins[] = {
    LED_1, LED_2, LED_3, LED_4, LED_5
};

static const size_t led_count = (sizeof(led_pins) / sizeof(led_pins[0]));

static nsec_led_effect current_effect = NSEC_LED_EFFECT_ALL_OFF;
static uint32_t current_delay = 0;

static void _nsec_led_timer_callback(void * context) {
    switch (current_effect) {
        case NSEC_LED_EFFECT_SPIN: {
            static int current_led = 0;
            nrf_gpio_pin_set(led_pins[current_led]);
            current_led = (current_led + 1) % led_count;
            nrf_gpio_pin_clear(led_pins[current_led]);
            }
            break;
        case NSEC_LED_EFFECT_FLASH_ALL:
            for(int i = 0; i < led_count; i++) {
                nrf_gpio_pin_toggle(led_pins[i]);
            }
            break;

        default:
            break;
    }
}

void nsec_led_set_delay(uint32_t milliseconds) {
    if(milliseconds != current_delay) {
        app_timer_stop(led_timer);
        APP_ERROR_CHECK(app_timer_start(led_timer, APP_TIMER_TICKS(milliseconds), NULL));
        current_delay = milliseconds;
    }
}

void nsec_led_init(void) {
    for(int i = 0; i < led_count; i++) {
        nrf_gpio_cfg_output(led_pins[i]);
        nrf_gpio_pin_set(led_pins[i]);
    }
    APP_ERROR_CHECK(app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, _nsec_led_timer_callback));
}

void nsec_led_set_effect(nsec_led_effect effect) {
    // Turn off all LED first
    for(int i = 0; i < led_count; i++) {
        nrf_gpio_pin_set(led_pins[i]);
    }
    current_effect = effect;
    switch (effect) {
        case NSEC_LED_EFFECT_ALL_ON:
            for(int i = 0; i < led_count; i++) {
                nrf_gpio_pin_clear(led_pins[i]);
            }
            break;

        default:
            break;
    }
}
