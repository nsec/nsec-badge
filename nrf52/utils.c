//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nrf.h>

uint8_t constrain_u8(uint8_t x, uint8_t a, uint8_t b)
{
	if (x >= a && x <= b) {
		return x;
	} else if (x < a) {
		return a;
	} else {
		return b;
	}
}

uint16_t constrain_u16(uint16_t x, uint16_t a, uint16_t b)
{
	if (x >= a && x <= b) {
		return x;
	} else if (x < a) {
		return a;
	} else {
		return b;
	}
}

uint32_t constrain_u32(uint32_t x, uint32_t a, uint32_t b)
{
	if (x >= a && x <= b) {
		return x;
	} else if (x < a) {
		return a;
	} else {
		return b;
	}
}