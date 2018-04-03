//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)
#include <nrf.h>
#include "nrf_sdh.h"

#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

#define NSEC_STRINGIFY_(...) #__VA_ARGS__
#define NSEC_STRINGIFY(...) NSEC_STRINGIFY_(__VA_ARGS__)

uint32_t constrain(uint32_t x, uint32_t a, uint32_t b);
int map(int x, int in_min, int in_max, int out_min, int out_max);
void led_show_error(ret_code_t rc, uint32_t delay);
