//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef controls_h
#define controls_h

#include <stdbool.h>

#define NSEC_CONTROLS_LIMIT_MAX_HANDLERS (64)
#define NSEC_CONTROLS_MAX_EVENT 8

typedef enum {
    BUTTON_UP,
    BUTTON_UP_RELEASE,
    BUTTON_DOWN,
    BUTTON_DOWN_RELEASE,
    BUTTON_BACK,
    BUTTON_BACK_RELEASE,
    BUTTON_ENTER,
    BUTTON_ENTER_RELEASE,
} button_t;

typedef void (*button_handler)(button_t button);

void nsec_controls_add_handler(button_handler handler);
void nsec_controls_suspend_handler(button_handler handler);
void nsec_controls_add_event(button_t button);
void nsec_controls_process(void);
void nsec_controls_enable(bool state);
bool is_press_action(button_t button);


#endif /* controls_h */
