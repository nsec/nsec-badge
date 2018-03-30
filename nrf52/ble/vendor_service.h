//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef vendor_service_h
#define vendor_service_h

#include <stdint.h>
#include <ble_gatts.h>


typedef struct {
	uint16_t handle;
	ble_uuid_t uuid;
} VendorService;


void create_vendor_service(VendorService* service);

void add_characteristic_to_vendor_service(VendorService* service, uint8_t value_size, ble_gatts_char_handles_t*
		characteristic_handles);


#endif //vendor_service_h
