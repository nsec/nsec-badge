//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include <sdk_errors.h>
#include "vendor_service.h"
#include "service_characteristic.h"
#include "abstract_advertiser.h"
#include "abstract_ble_observer.h"

ret_code_t create_ble_device(char* device_name);

void destroy_ble_device();

void configure_advertising();

void set_advertiser(struct Advertiser*);

void ble_start_advertising();

void ble_stop_advertising();

void add_observer(struct BleObserver*);

void ble_device_start_scan();

void ble_device_stop_scan();

uint32_t add_vendor_service(struct VendorService*);

bool ble_device_toggle_ble();

bool is_ble_enabled(void);

void ble_device_notify_characteristic(struct ServiceCharacteristic* characteristic, const uint8_t* value);
