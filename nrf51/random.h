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

#endif
