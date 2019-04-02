//  Copyright (c) 2019
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef input_box_h
#define input_box_h

#include <stdint.h>

typedef enum {
    input_DIRECTION_UP,
    input_DIRECTION_DOWN,
} input_DIRECTION;

void input_box_init(const char *text, const char *password);
#endif
