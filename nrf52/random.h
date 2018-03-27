//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdint.h>

void nsec_random_get(uint8_t * buffer, size_t buffer_size);
uint8_t nsec_random_get_byte(uint8_t max);
uint8_t nsec_random_get_byte_range(uint8_t min, uint8_t max);
uint16_t nsec_random_get_u16(uint16_t max);

#endif
