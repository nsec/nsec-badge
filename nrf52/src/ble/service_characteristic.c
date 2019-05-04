//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "service_characteristic.h"
#include <stddef.h>
#include <app_error.h>
#include "app/logs.h"
#include "uuid.h"


#define NO_CONNECTION_HANDLE_REQUIRED BLE_CONN_HANDLE_INVALID

void ble_device_notify_characteristic(struct ServiceCharacteristic* characteristic, const uint8_t* value);


void create_characteristic(struct ServiceCharacteristic* characteristic, uint16_t value_length, ReadMode read, WriteMode write, uint16_t uuid){
    characteristic->read_mode = read;
    characteristic->write_mode = write;
    characteristic->value_length = value_length;
    characteristic->on_write_operation_done = NULL;
    characteristic->on_write_request = NULL;
    characteristic->on_read_request = NULL;
    characteristic->uuid = (ble_uuid_t){uuid, TYPE_NSEC_UUID};
    characteristic->read_permission = READ_OPEN;
    characteristic->write_permission = WRITE_OPEN;
    characteristic->user_descriptor = NULL;
    characteristic->data_type = 0;
    characteristic->allow_notify = false;
}

void set_characteristic_permission(struct ServiceCharacteristic* characteristic, ReadPermission read_perm,
                                  WritePermission write_perm){
    characteristic->read_permission = read_perm;
    characteristic->write_permission = write_perm;
}

uint16_t set_characteristic_value(struct ServiceCharacteristic* characteristic, uint8_t* value_buffer){
    ble_gatts_value_t characteristic_value;
    characteristic_value.len = characteristic->value_length;
    characteristic_value.p_value = value_buffer;
    characteristic_value.offset = 0;
    APP_ERROR_CHECK(sd_ble_gatts_value_set(NO_CONNECTION_HANDLE_REQUIRED, characteristic->handle, &characteristic_value));
    if(characteristic->allow_notify)
        notify_characteristic_value(characteristic, value_buffer);
    return characteristic_value.len;
}

void notify_characteristic_value(struct ServiceCharacteristic* characteristic, uint8_t* value_buffer){
    ble_device_notify_characteristic(characteristic, value_buffer);
}

uint16_t get_characteristic_value(struct ServiceCharacteristic* characteristic, uint8_t* value_buffer){
    ble_gatts_value_t characteristic_value;
    characteristic_value.len = characteristic->value_length;
    characteristic_value.p_value = value_buffer;
    characteristic_value.offset = 0;
    APP_ERROR_CHECK(sd_ble_gatts_value_get(NO_CONNECTION_HANDLE_REQUIRED, characteristic->handle, &characteristic_value));
    return characteristic_value.len;
}

void add_write_operation_done_handler(struct ServiceCharacteristic* characteristic, on_characteristic_write_command event_handler){
    characteristic->on_write_operation_done = event_handler;
}

void add_write_request_handler(struct ServiceCharacteristic* characteristic, on_characteristic_write_request event_handler){
    characteristic->on_write_request = event_handler;
}

void add_read_request_handler(struct ServiceCharacteristic* characteristic, on_characteristic_read_request event_handler){
    characteristic->on_read_request = event_handler;
}
