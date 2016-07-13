//
//  controls.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-17.
//
//

#ifndef controls_h
#define controls_h

#include <stdbool.h>

#define NSEC_CONTROLS_LIMIT_MAX_HANDLERS (8)

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_ENTER,
    BUTTON_BACK,
} button_t;

typedef void (*button_handler)(button_t button);

void nsec_controls_add_handler(button_handler handler);

void nsec_controls_trigger(button_t button);

#endif /* controls_h */
