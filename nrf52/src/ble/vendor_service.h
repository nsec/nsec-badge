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


struct VendorService {
    uint16_t handle;
    ble_uuid_t uuid;
    uint16_t characteristic_count;
    struct ServiceCharacteristic* characteristics[MAX_CHARACTERISTICS_PER_SERVICE];
};


void create_vendor_service(struct VendorService* service, uint16_t uuid);

void add_characteristic_to_vendor_service(struct VendorService* service, struct ServiceCharacteristic* characteristic);

struct ServiceCharacteristic* get_characteristic(struct VendorService* service, uint16_t characteristic_uuid);


#endif //vendor_service_h
