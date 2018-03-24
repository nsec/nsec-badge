//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)
#include <nrf.h>


#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

uint8_t constrain_u8(uint8_t x, uint8_t a, uint8_t b);
uint16_t constrain_u16(uint16_t x, uint16_t a, uint16_t b);
uint32_t constrain_u32(uint32_t x, uint32_t a, uint32_t b);