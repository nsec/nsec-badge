//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef touch_button_h
#define touch_button_h

#include <stdint.h>

enum touch_event {
    TOUCH_EVENT_DOWN    = 0x01,
    TOUCH_EVENT_UP      = 0x02,
};

enum touch_button {
    TOUCH_BUTTON_UP     = 0x01,
    TOUCH_BUTTON_ENTER  = 0x02,
    TOUCH_BUTTON_RIGHT  = 0x03,
    TOUCH_BUTTON_LEFT   = 0x04,
    TOUCH_BUTTON_DOWN   = 0x05,
    TOUCH_BUTTON_BACK   = 0x06,
};

uint32_t touch_init(void);


#endif /* touch_button_h */
