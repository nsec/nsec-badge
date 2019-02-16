//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef _BLE_BEACON_H
#define _BLE_BEACON_H

#include <app_util.h>
#include <ble_advdata.h>


void set_broadcast_data(uint8_t* data, uint16_t size);
void set_beacon_name(const char*);
void set_advertising_interval_ms(uint16_t interval);
struct Advertiser* get_beacon();

#endif
