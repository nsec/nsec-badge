//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "controls.h"
#include <app_error.h>

struct handler {
    button_handler handler;
    bool active;
};

static struct handler handlers[NSEC_CONTROLS_LIMIT_MAX_HANDLERS];
static uint8_t handler_count = 0;

void nsec_controls_add_handler(button_handler handler) {
    if(handler_count >= NSEC_CONTROLS_LIMIT_MAX_HANDLERS) {
        return;
    }
    for(int i = 0; i < handler_count; i++) {
        if(handlers[i].handler == handler) {
            handlers[i].active = true;
            return;
        }
    }
    handlers[handler_count++] = (struct handler) {
        .handler = handler,
        .active = true
    };
}

void nsec_controls_suspend_handler(button_handler handler) {
    for(int i = 0; i < handler_count; i++) {
        if(handlers[i].handler == handler) {
            handlers[i].active = false;
            return;
        }
    }
}

void nsec_controls_trigger(button_t button) {
    for(int i = 0; i < handler_count; i++) {
        if (handlers[i].active) {
            handlers[i].handler(button);
        }
    }
}
