//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "boards.h"
#include "controls.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "led_effects.h"

void buttons_IRQHandler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
	switch (pin) {
		case INPUT_UP:
			nsec_controls_trigger(BUTTON_UP);
		break;

		case INPUT_DOWN:
			nsec_controls_trigger(BUTTON_DOWN);
		break;

		case INPUT_BACK:
			nsec_controls_trigger(BUTTON_BACK);
		break;

		case INPUT_ENTER:
			nsec_controls_trigger(BUTTON_ENTER);
		break;
	}
}

void buttons_init(void) {
	ret_code_t err_code;

	// Init the GPIOE module
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    // Create an input configuration
    // Pull UP
    // interrupt on 1 to 0 transition
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    // Apply config on every buttons
    err_code = nrf_drv_gpiote_in_init(INPUT_UP, &in_config, buttons_IRQHandler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(INPUT_DOWN, &in_config, buttons_IRQHandler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(INPUT_BACK, &in_config, buttons_IRQHandler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(INPUT_ENTER, &in_config, buttons_IRQHandler);
    APP_ERROR_CHECK(err_code);

    // Enable interrupt on every buttons
    nrf_drv_gpiote_in_event_enable(INPUT_UP, true);
    nrf_drv_gpiote_in_event_enable(INPUT_DOWN, true);
    nrf_drv_gpiote_in_event_enable(INPUT_BACK, true);
    nrf_drv_gpiote_in_event_enable(INPUT_ENTER, true);

}
