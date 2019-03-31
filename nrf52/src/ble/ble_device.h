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

void start_advertising();

void stop_advertising();

void add_observer(struct BleObserver*);

void ble_device_start_scan();

void ble_device_stop_scan();

uint32_t add_vendor_service(VendorService*);

void config_dummy_service(VendorService*, ServiceCharacteristic*);
