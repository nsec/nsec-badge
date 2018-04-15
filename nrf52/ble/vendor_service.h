//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef vendor_service_h
#define vendor_service_h

#include <stdint.h>
#include <ble_gatts.h>
#include "service_characteristic.h"


typedef struct {
	uint16_t handle;
	ble_uuid_t uuid;
} VendorService;


void create_vendor_service(VendorService* service);

uint32_t add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic);


#endif //vendor_service_h
