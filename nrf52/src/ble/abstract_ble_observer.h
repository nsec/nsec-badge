//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef NRF52_ABSTRACT_BLE_OBSERVER_H
#define NRF52_ABSTRACT_BLE_OBSERVER_H

#include <ble_gap.h>

struct BleObserver {
    void (*on_advertising_report)(const ble_gap_evt_adv_report_t*);
    void (*on_scan_timeout)(const ble_gap_evt_timeout_t*);
};

#endif //NRF52_ABSTRACT_BLE_OBSERVER_H
