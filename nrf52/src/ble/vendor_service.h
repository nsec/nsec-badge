//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef vendor_service_h
#define vendor_service_h

#include <stdint.h>
#include <ble_gatts.h>
#include "service_characteristic.h"

#define MAX_CHARACTERISTICS_PER_SERVICE 12


typedef struct {
    uint16_t handle;
    ble_uuid_t uuid;
    uint16_t characteristic_count;
    ServiceCharacteristic* characteristics[MAX_CHARACTERISTICS_PER_SERVICE];
} VendorService;


void create_vendor_service(VendorService* service, uint16_t uuid);

void add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic);

ServiceCharacteristic* get_characteristic(VendorService* service, uint16_t characteristic_uuid);


#endif //vendor_service_h
