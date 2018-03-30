//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nrf.h>

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
