//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef _BLE_ABSTRACT_ADVERTISER_H
#define _BLE_ABSTRACT_ADVERTISER_H

#include <ble.h>


struct Advertiser {
    void (*start_advertisement)();
    void (*stop_advertisement)();
    void (*on_ble_advertising_event)(ble_evt_t const * ble_event);
};

#endif
