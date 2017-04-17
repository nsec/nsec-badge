//
//  led_effects.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-04-15.
//
//

#include "led_effects.h"

#include <app_timer.h>
#include <nrf_gpio.h>
#include "boards.h"

APP_TIMER_DEF(led_timer);

static const uint8_t led_pins[] = {
    LED_1, LED_2, LED_3, LED_4, LED_5
};

static const size_t led_count = (sizeof(led_pins) / sizeof(led_pins[0]));

static void _nsec_led_timer_callback(void * context) {
    static int current_led = 0;
    nrf_gpio_pin_set(led_pins[current_led]);
    current_led = (current_led + 1) % led_count;
    nrf_gpio_pin_clear(led_pins[current_led]);
}

void nsec_led_set_delay(uint32_t milliseconds) {
    app_timer_stop(led_timer);
    APP_ERROR_CHECK(app_timer_start(led_timer, APP_TIMER_TICKS(milliseconds, 0), NULL));
}

void nsec_led_init(void) {
    for(int i = 0; i < led_count; i++) {
        nrf_gpio_cfg_output(led_pins[i]);
        nrf_gpio_pin_set(led_pins[i]);
    }
    APP_ERROR_CHECK(app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, _nsec_led_timer_callback));
}
