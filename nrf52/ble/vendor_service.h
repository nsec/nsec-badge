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

// Only the leftmost byte of the 16bit UUID is used to identify vendor services. The other byte is used for characteristics of the service
#define UUID_SERVICE_PART_OFFSET 8
#define UUID_SERVICE_PART_MASK 0xff00
#define UUID_CHARACTERISTIC_PART_MASK 0xff


typedef struct {
    uint16_t handle;
    ble_uuid_t uuid;
    uint16_t characteristic_count;
    ServiceCharacteristic* characteristics[MAX_CHARACTERISTICS_PER_SERVICE];
} VendorService;


void create_vendor_service(VendorService* service);

void add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic, uint16_t value_length, ReadMode read,
        WriteMode write);

void create_uuid_for_vendor_service(ble_uuid_t* service_uuid, uint16_t service_number);

ServiceCharacteristic* get_characteristic(VendorService* service, uint16_t chararacteristic_uuid);


#endif //vendor_service_h
