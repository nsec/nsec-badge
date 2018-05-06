//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "logs.h"
#include <app_error.h>
#include <ble.h>
#include "service_characteristic.h"


static void create_uuid_for_service_characteristic(VendorService* service, ble_uuid_t* char_uuid);


void create_vendor_service(VendorService* service){
    service->characteristic_count = 0;
}

void add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic, uint16_t value_length, ReadMode read,
        WriteMode write){
    if(service->characteristic_count >= MAX_CHARACTERISTICS_PER_SERVICE)
        return;
    ble_gatts_char_handles_t characteristic_handles;
    service->characteristics[service->characteristic_count] = characteristic;
    create_characteristic(characteristic, value_length, read, write);
    ble_gatts_char_md_t metadata;
    ble_gatts_attr_md_t attribute_metadata;
    ble_gatts_attr_t attribute;
    configure_characteristic(characteristic, &metadata, &attribute_metadata, &attribute);
    service->characteristic_count++;
    ble_uuid_t characteristic_uuid;
    create_uuid_for_service_characteristic(service, &characteristic_uuid);
    attribute.p_uuid = &characteristic_uuid;
    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(service->handle, &metadata, &attribute, &characteristic_handles));
    characteristic->handle = characteristic_handles.value_handle;
}

void create_uuid_for_vendor_service(ble_uuid_t* service_uuid, uint16_t service_number){
    ble_uuid128_t base_uuid = {{0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x00, 0x00, 0xCA, 0xCB}};
    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &service_uuid->type));
    service_uuid->uuid = service_number << 8;
}

ServiceCharacteristic* get_characteristic(VendorService* service, uint16_t chararacteristic_uuid){
    uint16_t characteristic_number = chararacteristic_uuid & UUID_CHARACTERISTIC_PART_MASK;
    if(service->characteristic_count >= characteristic_number)
        return service->characteristics[characteristic_number - 1];
    return NULL;
}

static void create_uuid_for_service_characteristic(VendorService* service, ble_uuid_t* char_uuid){
    char_uuid->type = service->uuid.type;
    char_uuid->uuid = service->uuid.uuid + service->characteristic_count;
}
