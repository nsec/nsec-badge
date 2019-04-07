//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "controls.h"

#include <app_error.h>
#include <nrf_queue.h>

struct handler {
    button_handler handler;
    bool active;
};

static struct handler handlers[NSEC_CONTROLS_LIMIT_MAX_HANDLERS];
static uint8_t handler_count = 0;
static bool enable = true;

NRF_QUEUE_DEF(button_t, event_queue, NSEC_CONTROLS_MAX_EVENT,
              NRF_QUEUE_MODE_NO_OVERFLOW);

bool is_press_action(button_t button)
{
    return button == BUTTON_UP || button == BUTTON_DOWN ||
        button == BUTTON_BACK || button == BUTTON_ENTER;
}

static void nsec_controls_trigger(button_t button) {
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].active) {
            handlers[i].handler(button);
        }
    }
}

void nsec_controls_add_handler(button_handler handler) {
    if (handler_count >= NSEC_CONTROLS_LIMIT_MAX_HANDLERS) {
        return;
    }

    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].handler == handler) {
            handlers[i].active = true;
            return;
        }
    }

    handlers[handler_count++] =
        (struct handler){.handler = handler, .active = true};
}

void nsec_controls_enable(bool state) {
    enable = state;
}

void nsec_controls_suspend_handler(button_handler handler) {
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].handler == handler) {
            handlers[i].active = false;
            return;
        }
    }
}

void nsec_controls_add_event(button_t button) {
    if (enable) {
        nrf_queue_push(&event_queue, &button);
    }
}

void nsec_controls_process(void) {
    button_t event;
    ret_code_t ret;

    while (true) {
        CRITICAL_REGION_ENTER();
        ret = nrf_queue_pop(&event_queue, &event);
        CRITICAL_REGION_EXIT();

        if (ret == NRF_ERROR_NOT_FOUND) {
            return;
        }
        nsec_controls_trigger(event);
    }
}
