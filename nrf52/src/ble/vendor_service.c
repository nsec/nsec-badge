//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "app/logs.h"
#include <app_error.h>
#include <ble.h>
#include "service_characteristic.h"
#include "uuid.h"


void create_vendor_service(struct VendorService* service, uint16_t uuid){
    service->characteristic_count = 0;
    service->uuid = (ble_uuid_t){uuid, TYPE_NSEC_UUID};
}

void add_characteristic_to_vendor_service(struct VendorService* service, struct ServiceCharacteristic* characteristic){
    if(service->characteristic_count >= MAX_CHARACTERISTICS_PER_SERVICE)
        return;
    ble_gatts_char_handles_t characteristic_handles;
    service->characteristics[service->characteristic_count] = characteristic;
    ble_gatts_char_md_t metadata;
    ble_gatts_attr_md_t attribute_metadata;
    ble_gatts_attr_t attribute;
    configure_characteristic(characteristic, &metadata, &attribute_metadata, &attribute);
    service->characteristic_count++;
    attribute.p_uuid = &(characteristic->uuid);
    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(service->handle, &metadata, &attribute, &characteristic_handles));
    characteristic->handle = characteristic_handles.value_handle;
}

struct ServiceCharacteristic* get_characteristic(struct VendorService* service, uint16_t characteristic_uuid){
    for(int i = 0; i < service->characteristic_count; i++){
        if(service->characteristics[i]->uuid.uuid == characteristic_uuid)
            return service->characteristics[i];
    }
    return NULL;
}
