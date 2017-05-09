//
//  battery_manager.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-17.
//
//

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
    app_timer_start(_battery_manager_timer_id, APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER), NULL);
    _nsec_battery_check(NULL);
}

static void _nsec_battery_check(void *p_context) {
    if (battery_is_undercharge()) {
        battery_disconnect();
    }
    else {
        battery_connect();
    }

    uint16_t voltage = battery_get_voltage();
    if(battery_is_charging()) {
        nsec_status_set_battery_status(STATUS_BATTERY_CHARGING);
    }
    else if(voltage > 4100) {
        nsec_status_set_battery_status(STATUS_BATTERY_100_PERCENT);
    }
    else if(voltage > 3900) {
        nsec_status_set_battery_status(STATUS_BATTERY_75_PERCENT);
    }
    else if(voltage > 3600) {
        nsec_status_set_battery_status(STATUS_BATTERY_50_PERCENT);
    }
    else if(!battery_is_undercharge()) {
        nsec_status_set_battery_status(STATUS_BATTERY_25_PERCENT);
    }
    else {
        nsec_status_set_battery_status(STATUS_BATTERY_0_PERCENT);
    }

    battery_refresh();
}
