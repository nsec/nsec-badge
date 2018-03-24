//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include "app_timer.h"
#include "nrf_drv_clock.h"

#define APP_TIMER_TIMEOUT     500
uint64_t timeout_count = 0;

/*
 * Task timers
 */
APP_TIMER_DEF(m_heartbeat_timer_id);
static void heartbeat_timeout_handler(void *p_context) {
    timeout_count++;
}

/*
 * Initialize the app timer library and heartbeat
 */
void timers_init(void) {
    ret_code_t err_code;

    // Initialize the application timer module
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_heartbeat_timer_id ,
                APP_TIMER_MODE_REPEATED,
                heartbeat_timeout_handler);

    APP_ERROR_CHECK(err_code);
}

void application_timers_start(void) {
    ret_code_t err_code;

    // Start application timers.
    //TODO confirm if 500ms is ok
    err_code = app_timer_start(m_heartbeat_timer_id,
                         APP_TIMER_TICKS(APP_TIMER_TIMEOUT /* ms */), NULL);
    APP_ERROR_CHECK(err_code);
}