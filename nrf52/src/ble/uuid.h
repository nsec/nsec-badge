//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include <stdint.h>
#include <ble.h>

extern uint8_t TYPE_NSEC_UUID;

void register_nsec_vendor_specific_uuid();

void register_custom_vendor_uuid(ble_uuid128_t* vendor_uuid, uint8_t* vendor_uuid_type);
