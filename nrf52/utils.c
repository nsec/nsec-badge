//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nrf.h>
#include <nrf_delay.h>
#include "nrf_sdh.h"
#include "ws2812fx.h"

// https://www.arduino.cc/reference/en/language/functions/math/constrain/
uint32_t constrain(uint32_t x, uint32_t a, uint32_t b)
{
	if (x >= a && x <= b) {
		return x;
	} else if (x < a) {
		return a;
	} else {
		return b;
	}
}

// https://www.arduino.cc/reference/en/language/functions/math/map/
int map(int x, int in_min, int in_max, int out_min, int out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* helper fonction for debugging
 * ws2812fx need to be init before
 */
void led_show_error(ret_code_t rc, uint32_t delay) {
	bool stop_before_quit = false;
	if (!isRunning_WS2812FX()) {
		start_WS2812FX();
	}

	if (rc == NRF_SUCCESS) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, GREEN, 1000, false);
    } else if (rc == NRF_ERROR_NULL) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, BLUE, 1000, false);
    } else if (rc == NRF_ERROR_INVALID_STATE) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, YELLOW, 1000, false);
    } else if (rc == NRF_ERROR_INVALID_LENGTH) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, CYAN, 1000, false);
    } else if (rc == NRF_ERROR_INVALID_ADDR) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, MAGENTA, 1000, false);
    } else if (rc == NRF_ERROR_NO_MEM) {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, PURPLE, 1000, false);
    } else {
		setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, RED, 1000, false);
    }
    service_WS2812FX();
    nrf_delay_ms(delay);

    setSegment_WS2812FX(0, 0, 7, FX_MODE_STATIC, BLACK, 1000, false);
    service_WS2812FX();

    if (stop_before_quit) {
    	stop_WS2812FX();
    }
}
