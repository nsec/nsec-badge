//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef vendor_service_h
#define vendor_service_h

#include <stdint.h>
#include <ble_gatts.h>
#include "service_characteristic.h"

#define MAX_CHARACTERISTICS_PER_SERVICE 8


typedef struct {
	uint16_t handle;
	ble_uuid_t uuid;
	uint16_t characteristic_count;
	ServiceCharacteristic characteristics[MAX_CHARACTERISTICS_PER_SERVICE];
} VendorService;


void create_vendor_service(VendorService* service);

uint32_t add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic, uint16_t value_length, bool read, bool write);

void create_uuid_for_vendor_service(ble_uuid_t* service_uuid, uint16_t service_number);


#endif //vendor_service_h
