//  Copyright (c) 2019
//  Eric Tremblay (habscup@gmail.com)
//
//  License: MIT (see LICENSE for details)

#include "button_service.h"
#include <stdint.h>
#include <stdio.h>

#include "ble/nsec_ble.h"
#include "ble/ble_device.h"
#include "ble/service_characteristic.h"
#include "ble/vendor_service.h"

#include "drivers/controls.h"
#include "drivers/buttons.h"

#include "app/cli.h"
#include "uuid.h"

static struct VendorService button_ble_service;
static struct ServiceCharacteristic up_characteristic;
static struct ServiceCharacteristic down_characteristic;
static struct ServiceCharacteristic enter_characteristic;
static struct ServiceCharacteristic back_characteristic;

static uint16_t service_uuid = 0x0020;   //ID
static uint16_t up_char_uuid = 0x0120; //characteristic 1 of ID, bytes are reversed
static uint16_t down_char_uuid = 0x0220; //characteristic 1 of ID, bytes are reversed
static uint16_t enter_char_uuid = 0x0320;  //characteristic 1 of ID, bytes are reversed
static uint16_t back_char_uuid = 0x0420;  //characteristic 1 of ID, bytes are reversed

static bool is_valid_action(button_t button, bool action)
{
    if (nsec_button_is_pushed(button) && action) {
        return false;
    }

    if (!nsec_button_is_pushed(button) && !action) {
        return false;
    }
    return true;
}

static uint16_t valid_button_event(const uint8_t *data, uint8_t length)
{
    if (length != 1) {
        return BLE_GATT_STATUS_ATTERR_INVALID_ATT_VAL_LENGTH;
    }

    if (data[0] != 0 && data[0] != 1) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    return 0;
}

static void set_button_char_value(button_t button, bool value)
{
    if (button == BUTTON_UP) {
        set_characteristic_value(&up_characteristic, (uint8_t*)&value);
    } else if (button == BUTTON_DOWN) {
        set_characteristic_value(&down_characteristic, (uint8_t*)&value);
    } else if (button == BUTTON_ENTER) {
        set_characteristic_value(&enter_characteristic, (uint8_t*)&value);
    } else {
        set_characteristic_value(&back_characteristic, (uint8_t*)&value);
    }
}

static uint16_t write_handler(CharacteristicWriteEvent* event)
{
    uint16_t status = valid_button_event(event->data_buffer, event->data_length);
    if (status != 0) {
        return status;
    }

    bool action = event->data_buffer[0];

    button_t button;
    if (event->uuid == up_char_uuid) {
        button = BUTTON_UP;
    } else if (event->uuid == down_char_uuid) {
        button = BUTTON_DOWN;
    } else if (event->uuid == enter_char_uuid) {
        button = BUTTON_ENTER;
    } else {
        button = BUTTON_BACK;
    }

    if (!is_valid_action(button, action)) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    if (action) {
        nsec_controls_add_event(button);
    } else {
        nsec_controls_add_event(button+1);
    }

    set_button_char_value(button, action);

    return BLE_GATT_STATUS_SUCCESS;
}


void init_button_service(void) {
    uint8_t init_value = 0;
    ble_uuid_t uuid = {.uuid = service_uuid, .type = TYPE_NSEC_UUID};
    create_vendor_service(&button_ble_service, &uuid);
    add_vendor_service(&button_ble_service);

    create_characteristic(
        &up_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, up_char_uuid);
    up_characteristic.user_descriptor = "Up";
    up_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &up_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &up_characteristic);
    add_write_request_handler(&up_characteristic, write_handler);
    set_characteristic_value(&up_characteristic, &init_value);

    create_characteristic(
        &down_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, down_char_uuid);
    down_characteristic.user_descriptor = "Down";
    down_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &down_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &down_characteristic);
    add_write_request_handler(&down_characteristic, write_handler);
    set_characteristic_value(&down_characteristic, &init_value);

    create_characteristic(
        &enter_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, enter_char_uuid);
    enter_characteristic.user_descriptor = "Enter";
    enter_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &enter_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &enter_characteristic);
    add_write_request_handler(&enter_characteristic, write_handler);
    set_characteristic_value(&enter_characteristic, &init_value);

    create_characteristic(
        &back_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, back_char_uuid);
    back_characteristic.user_descriptor = "Back";
    back_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &back_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &back_characteristic);
    add_write_request_handler(&back_characteristic, write_handler);
    set_characteristic_value(&back_characteristic, &init_value);
}
