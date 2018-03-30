//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "logs.h"

#include <ble.h>


static void get_uuid_for_vendor_service(ble_uuid_t* service_uuid);
static void set_default_metadata_for_characteristic(ble_gatts_char_md_t* metadata);
static void create_uuid_for_service_characteristic(ble_uuid_t* const service_uuid, ble_uuid_t* char_uuid);


void create_vendor_service(VendorService* service){
	get_uuid_for_vendor_service(&service->uuid);
	uint32_t error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service->uuid, &service->handle);
	log_error_code("sd_ble_gatts_service_add", error_code);
}

void add_characteristic_to_vendor_service(VendorService* service, uint8_t value_size, ble_gatts_char_handles_t*
		characteristic_handles){

	ble_gatts_attr_t ble_attribute;
	ble_gatts_char_md_t characteristic_metadata;
	ble_gatts_attr_md_t ble_attribute_metadata;
	ble_uuid_t characteristic_uuid;
	set_default_metadata_for_characteristic(&characteristic_metadata);
	bzero(&ble_attribute_metadata, sizeof(ble_attribute_metadata));
	ble_attribute_metadata.vloc = BLE_GATTS_VLOC_STACK;
	ble_attribute.init_len = value_size;
	ble_attribute.max_len = value_size;
	ble_attribute.init_offs = 0;
	ble_attribute.p_attr_md = &ble_attribute_metadata;
	create_uuid_for_service_characteristic(&service->uuid, &characteristic_uuid);
	ble_attribute.p_uuid = &characteristic_uuid;
	uint32_t error_code = sd_ble_gatts_characteristic_add(service->handle, &characteristic_metadata, &ble_attribute,
			&characteristic_handles);
	log_error_code("sd_ble_gatts_characteristic_add", error_code);
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

static void set_default_metadata_for_characteristic(ble_gatts_char_md_t* metadata){
	metadata->char_props.read   = 1;
	metadata->char_props.notify = 1;
	metadata->char_props.write = 1;
	metadata->char_props.write_wo_resp = 1;
	metadata->p_char_user_desc  = NULL;
	metadata->p_char_pf         = NULL;
	metadata->p_user_desc_md    = NULL;
	metadata->p_cccd_md         = NULL;
	metadata->p_sccd_md         = NULL;
}
