//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "battery.h"
#include <app_timer.h>
#include <nrf_gpio.h>
#include "status_bar.h"
#include "boards.h"

APP_TIMER_DEF(_battery_manager_timer_id);
static void _nsec_battery_check(void *p_context);

void nsec_battery_manager_init(void) {
    battery_init();

    app_timer_create(&_battery_manager_timer_id,
                     APP_TIMER_MODE_REPEATED,
                     _nsec_battery_check);
    app_timer_start(_battery_manager_timer_id,
            APP_TIMER_TICKS(1000/*ms*/), NULL);

    _nsec_battery_check(NULL);
}

static void _nsec_battery_check(void *p_context) {
    if (!battery_is_present()) {
        nsec_status_set_battery_status(STATUS_BATTERY_0_PERCENT);
        return;
    }

    uint16_t voltage = battery_get_voltage();

    if(voltage > 131) {
        nsec_status_set_battery_status(STATUS_BATTERY_100_PERCENT);
    }
    else if(voltage > 130) {
        nsec_status_set_battery_status(STATUS_BATTERY_75_PERCENT);
    }
    else if(voltage > 129) {
        nsec_status_set_battery_status(STATUS_BATTERY_50_PERCENT);
    }
    else if(voltage > 128) {
        nsec_status_set_battery_status(STATUS_BATTERY_25_PERCENT);
    }
    else {
        nsec_status_set_battery_status(STATUS_BATTERY_0_PERCENT);
    }

    battery_refresh();
}
