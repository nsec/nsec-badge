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

typedef void (*on_characteristic_write_command)(CharacteristicWriteEvent*);

typedef uint16_t (*on_characteristic_write_request)(CharacteristicWriteEvent*);

typedef uint16_t (*on_characteristic_read_request)(CharacteristicReadEvent*);

typedef enum {
	AUTO_READ,    // Reading of a characteristic by a BLE client is done automatically by the soft device and the application is not notified.
	REQUEST_READ, // Reading of a characteristic by a BLE client calls on_read_request of the characteristic, which can allow or deny the read request.
	DENY_READ     // All read requests will be denied by the soft device.
} ReadMode;

typedef enum {
	AUTO_WRITE,    // Writing into a characteristic by a BLE client is done automatically by the soft device and the on_write_command of the characteristic is called.
	REQUEST_WRITE, // WRiting into a characteristic by a BLE client calls on_write_request of the characteristic, which can allow or deny the write request, and choose the written value.
	DENY_WRITE     // All write requests will be denied by the soft device.
} WriteMode;

typedef struct {
	uint16_t handle;
	uint16_t value_length;
	ReadMode read_mode;
	WriteMode write_mode;
	on_characteristic_write_command on_write_command;
	on_characteristic_write_request on_write_request;
	on_characteristic_read_request on_read_request;
} ServiceCharacteristic;


void create_characteristic(ServiceCharacteristic* characteristic, uint16_t value_length, ReadMode read, WriteMode write);

void configure_characteristic(ServiceCharacteristic* characteristic, ble_gatts_char_md_t* metadata,
		ble_gatts_attr_md_t* attribute_metadata, ble_gatts_attr_t* attribute);

uint16_t set_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);

uint16_t get_characteristic_value(ServiceCharacteristic*, uint8_t* value_buffer);

void add_write_command_handler(ServiceCharacteristic*, on_characteristic_write_command);

void add_write_request_handler(ServiceCharacteristic*, on_characteristic_write_request);

void add_read_request_handler(ServiceCharacteristic*, on_characteristic_read_request);


#endif //service_characteristic_h
