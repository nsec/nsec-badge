//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "uuid.h"
#include <ble.h>
#include <app_error.h>


uint8_t TYPE_NSEC_UUID = 0;

/*
 * Register the vendor specific 128 bit UUID of NorthSec in the BLE stack to allow services and characteristics to
 * use a 16 bit UUID by setting ble_uuid_t.type to TYPE_NSEC_UUID.
 */
void register_nsec_vendor_specific_uuid(){
    ble_uuid128_t base_uuid = {{0x4d, 0x58, 0x7b, 0x26, 0x72, 0x2a, 0x49, 0x3a, 0x8e, 0x72, 0x8e, 0x9d, 0xc9, 0x41, 0x0d, 0xa1}};
    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &TYPE_NSEC_UUID));
}
