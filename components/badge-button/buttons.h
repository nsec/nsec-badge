/* SPDX-FileCopyrightText: 2024 NorthSec
 *
 * SPDX-License-Identifier: MIT */

#ifndef NSEC_TASKS_BUTTONS_H
#define NSEC_TASKS_BUTTONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Badge Button. */
typedef enum {
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_OK,
    BUTTON_CANCEL,
    BUTTON_TOTAL_COUNT,
} badge_button_t;

/* Badge Button events. */
typedef enum {
    BADGE_BUTTON_SINGLE_CLICK = 0,
    BADGE_BUTTON_LONG_PRESS,
    BADGE_BUTTON_TOTAL_COUNT,
    BADGE_BUTTON_NO_EVENT = BADGE_BUTTON_TOTAL_COUNT
} badge_button_event_t;

/* Task callback definition. */
typedef void (* badge_button_event_cb_t)(badge_button_t, badge_button_event_t);

void buttons_init(void);
void buttons_register_ca(badge_button_event_cb_t cb);
void buttons_task(void *arg __attribute__((unused)));

#ifdef __cplusplus
}
#endif

#endif
