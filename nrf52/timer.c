//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include <app_timer.h>
#include <nrf_drv_clock.h>

#include "timer.h"

uint64_t heartbeat_timeout_count = 0;

APP_TIMER_DEF(m_heartbeat_timer_id);

/*
 * Callback function when the heartbeat timeout expires
 */
static
void heartbeat_timeout_handler(void *p_context) {
    heartbeat_timeout_count++;
}

/*
 * Initialize the app timer library and heartbeat
 */
void timer_init(void) {
    ret_code_t err_code;

    // Initialize the application timer module
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create the heartbeat timer
    err_code = app_timer_create(&m_heartbeat_timer_id,
                APP_TIMER_MODE_REPEATED,
                heartbeat_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start the heartbeat timer
    //TODO confirm if 500ms is ok
    err_code = app_timer_start(m_heartbeat_timer_id,
                APP_TIMER_TICKS(HEARTBEAT_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);
}

/*
 * Get the elapsed time since startup in milliseconds
 */
uint64_t get_current_time_millis(void) {
    return (app_timer_cnt_get() / APP_TIMER_CLOCK_FREQ) +
           (HEARTBEAT_TIMER_TIMEOUT * heartbeat_timeout_count);
}
