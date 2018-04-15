//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "service_characteristic.h"

ret_code_t create_ble_device(char* device_name);

void destroy_ble_device();

void configure_advertising();

void start_advertising();

void config_dummy_service(VendorService*, ServiceCharacteristic* characteristic);
