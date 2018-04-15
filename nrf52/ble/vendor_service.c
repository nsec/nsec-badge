//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "logs.h"

#include <ble.h>
#include "service_characteristic.h"


static void get_uuid_for_vendor_service(ble_uuid_t* service_uuid);
static void create_uuid_for_service_characteristic(ble_uuid_t* const service_uuid, ble_uuid_t* char_uuid);


void create_vendor_service(VendorService* service){
	get_uuid_for_vendor_service(&service->uuid);
	uint32_t error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service->uuid, &service->handle);
	log_error_code("sd_ble_gatts_service_add", error_code);
}

uint32_t add_characteristic_to_vendor_service(VendorService* service, ServiceCharacteristic* characteristic){
	ble_gatts_char_handles_t characteristic_handles;
	ble_uuid_t characteristic_uuid;
	create_uuid_for_service_characteristic(&service->uuid, &characteristic_uuid);
	characteristic->attribute.p_uuid = &characteristic_uuid;
	uint32_t error_code = sd_ble_gatts_characteristic_add(service->handle, &characteristic->metadata,
			&characteristic->attribute, &characteristic_handles);
	characteristic->handle = characteristic_handles.value_handle;
	NRF_LOG_INFO("Value handle is %d", characteristic->handle);
	return error_code;
}

static void get_uuid_for_vendor_service(ble_uuid_t* service_uuid){
	ble_uuid128_t base_uuid = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x00, 0x00, 0xCA, 0xCB};
	log_error_code("sd_ble_uuid_vs_add", sd_ble_uuid_vs_add(&base_uuid, &service_uuid->type));
	service_uuid->uuid = 0;
}

static void create_uuid_for_service_characteristic(ble_uuid_t* const service_uuid, ble_uuid_t* char_uuid){
	char_uuid->type = service_uuid->type;
	char_uuid->uuid = 1;
}
