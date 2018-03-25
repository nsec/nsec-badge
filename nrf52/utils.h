//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)
#include <nrf.h>


#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

uint32_t constrain(uint32_t x, uint32_t a, uint32_t b);
int map(int x, int in_min, int in_max, int out_min, int out_max);
