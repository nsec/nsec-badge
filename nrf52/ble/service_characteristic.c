//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "service_characteristic.h"
#include <stddef.h>
#include <app_error.h>
#include "../logs.h"


#define NO_CONNECTION_HANDLE_REQUIRED BLE_CONN_HANDLE_INVALID


static void set_metadata_for_characteristic(ServiceCharacteristic*, ble_gatts_char_md_t*);
static void set_default_metadata_for_attribute(ble_gatts_attr_md_t* attribute_metadata);
static void set_attribute(ServiceCharacteristic*, ble_gatts_attr_t*, ble_gatts_attr_md_t*);


void create_characteristic(ServiceCharacteristic* characteristic, uint16_t value_length, bool read, bool write){
	characteristic->read = read;
	characteristic->write = write;
	characteristic->value_length = value_length;
	characteristic->on_write = NULL;
}

void configure_characteristic(ServiceCharacteristic* characteristic, ble_gatts_char_md_t* metadata,
		ble_gatts_attr_md_t* attribute_metadata, ble_gatts_attr_t* attribute){
	set_metadata_for_characteristic(characteristic, metadata);
	set_attribute(characteristic, attribute, attribute_metadata);
}

uint16_t set_characteristic_value(ServiceCharacteristic* characteristic, uint8_t* value_buffer){
	ble_gatts_value_t characteristic_value;
	characteristic_value.len = characteristic->value_length;
	characteristic_value.p_value = value_buffer;
	characteristic_value.offset = 0;
	APP_ERROR_CHECK(sd_ble_gatts_value_set(NO_CONNECTION_HANDLE_REQUIRED, characteristic->handle, &characteristic_value));
	return characteristic_value.len;
}

uint16_t get_characteristic_value(ServiceCharacteristic* characteristic, uint8_t* value_buffer){
	ble_gatts_value_t characteristic_value;
	characteristic_value.len = characteristic->value_length;
	characteristic_value.p_value = value_buffer;
	characteristic_value.offset = 0;
	APP_ERROR_CHECK(sd_ble_gatts_value_get(NO_CONNECTION_HANDLE_REQUIRED, characteristic->handle, &characteristic_value));
	return characteristic_value.len;
}

void add_write_event_handler(ServiceCharacteristic* characteristic, on_characteristic_write event_handler){
	characteristic->on_write = event_handler;
}

static void set_metadata_for_characteristic(ServiceCharacteristic* characteristic, ble_gatts_char_md_t* metadata){
	bzero(metadata, sizeof(*metadata));
	metadata->char_props.read = characteristic->read;
	metadata->char_props.write = characteristic->write;
	metadata->p_char_user_desc = NULL;
	metadata->p_char_pf = NULL;
	metadata->p_user_desc_md = NULL;
	metadata->p_cccd_md = NULL;
	metadata->p_sccd_md = NULL;
}

static void set_attribute(ServiceCharacteristic* characteristic, ble_gatts_attr_t* attribute, ble_gatts_attr_md_t* attribute_metadata){
	attribute->init_len = characteristic->value_length;
	attribute->max_len = characteristic->value_length;
	attribute->init_offs = 0;
	set_default_metadata_for_attribute(attribute_metadata);
	attribute->p_attr_md = attribute_metadata;
}

static void set_default_metadata_for_attribute(ble_gatts_attr_md_t* attribute_metadata){
	bzero(attribute_metadata, sizeof(*attribute_metadata));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attribute_metadata->read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attribute_metadata->write_perm);
	attribute_metadata->vloc = BLE_GATTS_VLOC_STACK;
	attribute_metadata->rd_auth = 0;
	attribute_metadata->wr_auth = 0;
	attribute_metadata->vlen = 0;
}
