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
	ble_uuid_t characteristic_uuid;
	uint16_t write_offset;
	uint16_t data_length;
	uint16_t characteristic_handle;
	uint8_t* data_buffer;
} CharacteristicWriteEvent;

typedef struct {
	ble_uuid_t characteristic_uuid;
	uint16_t read_offset;
	uint16_t characteristic_handle;
} CharacteristicReadEvent;

typedef void (*on_characteristic_write)(CharacteristicWriteEvent*);

typedef uint16_t (*on_characteristic_write_request)(CharacteristicWriteEvent*);

typedef uint16_t (*on_characteristic_read_request)(CharacteristicReadEvent*);

typedef struct {
	uint16_t handle;
	uint16_t value_length;
	bool read;
	bool write;
	on_characteristic_write on_write;
	on_characteristic_write_request on_write_request;
	on_characteristic_read_request on_read_request;
} ServiceCharacteristic;


void create_characteristic(ServiceCharacteristic* characteristic, uint16_t value_length, bool read, bool write,
		bool auto_read, bool auto_write);

void configure_characteristic(ServiceCharacteristic* characteristic, ble_gatts_char_md_t* metadata,
		ble_gatts_attr_md_t* attribute_metadata, ble_gatts_attr_t* attribute, bool auto_read, bool auto_write);

uint16_t set_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);

uint16_t get_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);

void add_write_event_handler(ServiceCharacteristic* characteristic, on_characteristic_write event_handler);

void add_write_request_handler(ServiceCharacteristic*, on_characteristic_write_request);

void add_read_request_handler(ServiceCharacteristic*, on_characteristic_read_request);


#endif //service_characteristic_h
