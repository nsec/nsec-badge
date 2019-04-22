/*
 * Copyright 2016-2017 Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 *           2016-2017 Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
 *           2018 Michael Jeanson <mjeanson@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <app_timer.h>
#include <nrf_gpio.h>

#include "app/status_bar.h"
#include "app/timer.h"
#include "battery.h"
#include "boards.h"

#define BATTERY_HISTERESIS_MV 20

#define BATTERY_100PER_THRES_MV 3800
#define BATTERY_75PER_THRES_MV 3700
#define BATTERY_50PER_THRES_MV 3300
#define BATTERY_25PER_THRES_MV 3000

#define BATTERY_100PER_UP_THRES_MV                                             \
    (BATTERY_100PER_THRES_MV + BATTERY_HISTERESIS_MV)
#define BATTERY_75PER_UP_THRES_MV                                              \
    (BATTERY_75PER_THRES_MV + BATTERY_HISTERESIS_MV)
#define BATTERY_50PER_UP_THRES_MV                                              \
    (BATTERY_50PER_THRES_MV + BATTERY_HISTERESIS_MV)
#define BATTERY_25PER_UP_THRES_MV                                              \
    (BATTERY_25PER_THRES_MV + BATTERY_HISTERESIS_MV)

#define BATTERY_100PER_DOWN_THRES_MV                                           \
    (BATTERY_100PER_THRES_MV - BATTERY_HISTERESIS_MV)
#define BATTERY_75PER_DOWN_THRES_MV                                            \
    (BATTERY_100PER_THRES_MV - BATTERY_HISTERESIS_MV)
#define BATTERY_50PER_DOWN_THRES_MV                                            \
    (BATTERY_100PER_THRES_MV - BATTERY_HISTERESIS_MV)
#define BATTERY_25PER_DOWN_THRES_MV                                            \
    (BATTERY_100PER_THRES_MV - BATTERY_HISTERESIS_MV)
static bool manager_event = false;
/*
 * This handler updates the battery icon in the status bar.
 */

void battery_manager_handler(void) { manager_event = true; }

void battery_manager_process(void) {

    if (manager_event) {
        manager_event = false;
    } else {
        return;
    }

    static uint16_t prev_voltage = 0;
    uint16_t voltage = battery_get_voltage();

// TODO can we detect that we are charging by comparing ADC values ?
#ifdef BOARD_SPUTNIK
    if (battery_is_usb_plugged()) {
        nsec_status_set_battery_status(STATUS_BATTERY_CHARGING);
        goto end_refresh;
    }
#endif

    if (voltage > prev_voltage) {
        if (voltage > BATTERY_100PER_UP_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_100_PERCENT);
        } else if (voltage > BATTERY_75PER_UP_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_75_PERCENT);
        } else if (voltage > BATTERY_50PER_UP_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_50_PERCENT);
        } else if (voltage > BATTERY_25PER_UP_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_25_PERCENT);
        } else {
            nsec_status_set_battery_status(STATUS_BATTERY_0_PERCENT);
        }
    } else {
        if (voltage > BATTERY_100PER_DOWN_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_100_PERCENT);
        } else if (voltage > BATTERY_75PER_DOWN_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_75_PERCENT);
        } else if (voltage > BATTERY_50PER_DOWN_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_50_PERCENT);
        } else if (voltage > BATTERY_25PER_DOWN_THRES_MV) {
            nsec_status_set_battery_status(STATUS_BATTERY_25_PERCENT);
        } else {
            nsec_status_set_battery_status(STATUS_BATTERY_0_PERCENT);
        }
    }

#ifdef BOARD_SPUTNIK
end_refresh:
#endif

    battery_refresh();
}

void nsec_battery_manager_init(void) {
    static bool is_init = false;
    if (is_init) {
        return;
    }

    is_init = true;

    battery_init();

    start_battery_manage_timer();

    /* Call the handler immediately to get the initial battery status */
    //_battery_manager_handler(NULL);
    battery_manager_handler();
}
