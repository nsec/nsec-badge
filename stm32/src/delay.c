//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <stdint.h>

#include "delay.h"

void delay(uint32_t cycle) {
    for (uint32_t i=0; i<cycle; i++) {
        __asm__("nop");
    }
}
