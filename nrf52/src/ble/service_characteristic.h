//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#ifndef service_characteristic_h
#define service_characteristic_h

#include <stdint.h>
#include <strings.h>
#include <ble_gatts.h>
#include <stdbool.h>


typedef struct {
    uint16_t write_offset;
    uint16_t data_length;
    const uint8_t* data_buffer;
} CharacteristicWriteEvent;

typedef struct {
    uint16_t read_offset;
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
    WRITE_REQUEST,      // Only write requests are allowed (i.e. write with acknowledgment). All write requests are processed and approved by the softdevice. The on_done_write_request callback is called after the characteristic value is updated.
    AUTH_WRITE_REQUEST, // Only write requests are allowed (i.e. write with acknowledgment). The on_write_request callback is invoked before any operation is performed, and the application must authorize the write operation. The application may change the characteristic value if desired.
    DENY_WRITE          // All write requests will be denied by the soft device. No callback is invoked.
} WriteMode;

struct ServiceCharacteristic {
    uint16_t handle;
    uint16_t value_length;
    ble_uuid_t uuid;
    ReadMode read_mode;
    WriteMode write_mode;
    on_characteristic_write_command on_write_operation_done;
    on_characteristic_write_request on_write_request;
    on_characteristic_read_request on_read_request;
};


void create_characteristic(struct ServiceCharacteristic* characteristic, uint16_t value_length, ReadMode read, WriteMode write, uint16_t uuid);

void configure_characteristic(struct ServiceCharacteristic* characteristic, ble_gatts_char_md_t* metadata,
        ble_gatts_attr_md_t* attribute_metadata, ble_gatts_attr_t* attribute);

uint16_t set_characteristic_value(struct ServiceCharacteristic*, uint8_t* value_buffer);

uint16_t get_characteristic_value(struct ServiceCharacteristic*, uint8_t* value_buffer);

void add_write_operation_done_handler(struct ServiceCharacteristic*, on_characteristic_write_command);

void add_write_request_handler(struct ServiceCharacteristic*, on_characteristic_write_request);

void add_read_request_handler(struct ServiceCharacteristic*, on_characteristic_read_request);


#endif //service_characteristic_h
