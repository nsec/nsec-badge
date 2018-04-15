//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#ifndef service_characteristic_h
#define service_characteristic_h

#include <stdint.h>
#include <ble_gatts.h>
#include <stdbool.h>


typedef struct {
	uint16_t handle;
	uint16_t value_length;
	ble_gatts_char_md_t metadata;
	ble_gatts_attr_md_t attribute_metadata;
	ble_gatts_attr_t attribute;
	bool read;
	bool write;
} ServiceCharacteristic;

void create_characteristic(ServiceCharacteristic* characteristic, uint16_t value_length, bool read, bool write);

void configure_characteristic(ServiceCharacteristic*);

uint16_t set_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);

uint16_t get_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);


#endif //service_characteristic_h
