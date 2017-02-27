/*
 * Copyright (c) 2015, Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 * Copyright (c) 2017, Marc-Etienne M.Léveillé <marc.etienne.ml@gmail.com>
 * License: MIT (see LICENSE for details)
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdint.h>

void nsec_random_get(uint8_t * buffer, size_t buffer_size);

#endif
