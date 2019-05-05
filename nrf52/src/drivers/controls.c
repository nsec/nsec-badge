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
static bool controls_enabled = true;

NRF_QUEUE_DEF(button_t, event_queue, NSEC_CONTROLS_MAX_EVENT,
              NRF_QUEUE_MODE_NO_OVERFLOW);

bool is_press_action(button_t button)
{
    return button == BUTTON_UP || button == BUTTON_DOWN ||
           button == BUTTON_BACK || button == BUTTON_ENTER;
}

/*
 * Execute all the active handlers.
 */
static void nsec_controls_trigger(button_t button)
{
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].active) {
            handlers[i].handler(button);
        }
    }
}

/*
 * Clear the handler array.
 */
void nsec_controls_clear_handlers(void)
{
    /* Purge all queued events */
    CRITICAL_REGION_ENTER();
    nrf_queue_reset(&event_queue);
    CRITICAL_REGION_EXIT();

    /* Clear the handler array */
    handler_count = 0;
    memset(handlers, 0, sizeof(handlers));
}

bool nsec_controls_add_handler(button_handler handler)
{
    if (handler_count >= NSEC_CONTROLS_LIMIT_MAX_HANDLERS) {
        return false;
    }

    /* Re-adding an existant handler, will enable it if it was disabled */
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].handler == handler) {
            handlers[i].active = true;
            return true;
        }
    }

    handlers[handler_count++] =
        (struct handler){.handler = handler, .active = true};

    return true;
}

/*
 * Disable control events queuing.
 */
void nsec_controls_enable(void)
{
    controls_enabled = true;
}

/*
 * Enable control events queuing.
 */
void nsec_controls_disable(void)
{
    controls_enabled = false;
}

/*
 * Add an enabled handler to the array.
 */
void nsec_controls_suspend_handler(button_handler handler)
{
    for (int i = 0; i < handler_count; i++) {
        if (handlers[i].handler == handler) {
            handlers[i].active = false;
            return;
        }
    }
}

/*
 * Add a button event to the queue if the controls are enabled.
 */
void nsec_controls_add_event(button_t button)
{
    if (controls_enabled) {
        nrf_queue_push(&event_queue, &button);
    }
}

/*
 * Process all the events in the queue.
 */
void nsec_controls_process(void)
{
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
