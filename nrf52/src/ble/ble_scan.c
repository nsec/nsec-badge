//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "ble_scan.h"

#include <app_error.h>
#include <app_util.h>
#include <ble_gap.h>


static ble_gap_scan_params_t params;

void configure_scan(bool active_scan, uint16_t timeout_s, uint16_t interval_ms, uint16_t window_ms){
    params.active = active_scan ? 1 : 0;
    params.adv_dir_report = 1;
    params.use_whitelist = 0;
    params.interval = MSEC_TO_UNITS(interval_ms, UNIT_0_625_MS);
    params.window = MSEC_TO_UNITS(window_ms, UNIT_0_625_MS);
    params.timeout = timeout_s;
}

void start_scan(){
    APP_ERROR_CHECK(sd_ble_gap_scan_start(&params));
}

void stop_scan(){
    APP_ERROR_CHECK(sd_ble_gap_scan_stop());
}
