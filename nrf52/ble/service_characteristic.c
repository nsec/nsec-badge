//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "service_characteristic.h"
#include <stddef.h>
#include <app_error.h>
#include "../logs.h"


#define NO_CONNECTION_HANDLE_REQUIRED BLE_CONN_HANDLE_INVALID


static void set_metadata_for_characteristic(ServiceCharacteristic*);
static void set_default_metadata_for_attribute(ServiceCharacteristic*);
static void set_attribute(ServiceCharacteristic*);


void create_characteristic(ServiceCharacteristic* characteristic, uint16_t value_length, bool read, bool write){
	characteristic->read = read;
	characteristic->write = write;
	characteristic->value_length = value_length;
	characteristic->on_write = NULL;
}

void configure_characteristic(ServiceCharacteristic* characteristic){
	set_metadata_for_characteristic(characteristic);
	set_attribute(characteristic);
	set_default_metadata_for_attribute(characteristic);
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

static void set_metadata_for_characteristic(ServiceCharacteristic* characteristic){
	bzero(&characteristic->metadata, sizeof(characteristic->metadata));
	characteristic->metadata.char_props.read = characteristic->read;
	characteristic->metadata.char_props.write = characteristic->write;
	characteristic->metadata.p_char_user_desc = NULL;
	characteristic->metadata.p_char_pf = NULL;
	characteristic->metadata.p_user_desc_md = NULL;
	characteristic->metadata.p_cccd_md = NULL;
	characteristic->metadata.p_sccd_md = NULL;
}

static void set_attribute(ServiceCharacteristic* characteristic){
	characteristic->attribute.init_len = characteristic->value_length;
	characteristic->attribute.max_len = characteristic->value_length;
	characteristic->attribute.init_offs = 0;
	characteristic->attribute.p_attr_md = &characteristic->attribute_metadata;
}

static void set_default_metadata_for_attribute(ServiceCharacteristic* characteristic){
	bzero(&characteristic->attribute_metadata, sizeof(characteristic->attribute_metadata));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristic->attribute_metadata.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristic->attribute_metadata.write_perm);
	characteristic->attribute_metadata.vloc = BLE_GATTS_VLOC_STACK;
	characteristic->attribute_metadata.rd_auth = 0;
	characteristic->attribute_metadata.wr_auth = 0;
	characteristic->attribute_metadata.vlen = 0;
}
