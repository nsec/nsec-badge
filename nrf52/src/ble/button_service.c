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

static uint16_t up_write_handler(CharacteristicWriteEvent* event)
{
    uint16_t status = valid_button_event(event->data_buffer, event->data_length);
    if (status != 0) {
        return status;
    }

    bool action = event->data_buffer[0];

    if (!is_valid_action(BUTTON_UP, action)) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    if (action) {
        nsec_controls_add_event(BUTTON_UP);
    } else {
        nsec_controls_add_event(BUTTON_UP_RELEASE);
    }

    set_characteristic_value(&up_characteristic, (uint8_t*)&action);

    return BLE_GATT_STATUS_SUCCESS;
}

static uint16_t down_write_handler(CharacteristicWriteEvent* event)
{
    uint16_t status = valid_button_event(event->data_buffer, event->data_length);
    if (status != 0) {
        return status;
    }

    bool action = event->data_buffer[0];

    if (!is_valid_action(BUTTON_DOWN, action)) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    if (action) {
        nsec_controls_add_event(BUTTON_DOWN);
    } else {
        nsec_controls_add_event(BUTTON_DOWN_RELEASE);
    }

    set_characteristic_value(&down_characteristic, (uint8_t*)&action);

    return BLE_GATT_STATUS_SUCCESS;
}

static uint16_t enter_write_handler(CharacteristicWriteEvent* event)
{
    uint16_t status = valid_button_event(event->data_buffer, event->data_length);
    if (status != 0) {
        return status;
    }

    bool action = event->data_buffer[0];

    if (!is_valid_action(BUTTON_ENTER, action)) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    if (action) {
        nsec_controls_add_event(BUTTON_ENTER);
    } else {
        nsec_controls_add_event(BUTTON_ENTER_RELEASE);
    }

    set_characteristic_value(&enter_characteristic, (uint8_t*)&action);

    return BLE_GATT_STATUS_SUCCESS;
}

static uint16_t back_write_handler(CharacteristicWriteEvent* event)
{
    uint16_t status = valid_button_event(event->data_buffer, event->data_length);
    if (status != 0) {
        return status;
    }

    bool action = event->data_buffer[0];

    if (!is_valid_action(BUTTON_BACK, action)) {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }

    if (action) {
        nsec_controls_add_event(BUTTON_BACK);
    } else {
        nsec_controls_add_event(BUTTON_BACK_RELEASE);
    }

    set_characteristic_value(&back_characteristic, (uint8_t*)&action);

    return BLE_GATT_STATUS_SUCCESS;
}

void init_button_service(void) {
    uint8_t init_value = 0;
    create_vendor_service(&button_ble_service, service_uuid);
    add_vendor_service(&button_ble_service);

    create_characteristic(
        &up_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, up_char_uuid);
    up_characteristic.user_descriptor = "Up";
    up_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &up_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &up_characteristic);
    add_write_request_handler(&up_characteristic, up_write_handler);
    set_characteristic_value(&up_characteristic, &init_value);

    create_characteristic(
        &down_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, down_char_uuid);
    up_characteristic.user_descriptor = "Down";
    up_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &down_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &down_characteristic);
    add_write_request_handler(&down_characteristic, down_write_handler);
    set_characteristic_value(&down_characteristic, &init_value);

    create_characteristic(
        &enter_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, enter_char_uuid);
    up_characteristic.user_descriptor = "Enter";
    up_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &enter_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &enter_characteristic);
    add_write_request_handler(&enter_characteristic, enter_write_handler);
    set_characteristic_value(&enter_characteristic, &init_value);

    create_characteristic(
        &back_characteristic, 1, DENY_READ, AUTH_WRITE_REQUEST, back_char_uuid);
    up_characteristic.user_descriptor = "Back";
    up_characteristic.data_type = BLE_GATT_CPF_FORMAT_BOOLEAN;
    set_characteristic_permission(
        &back_characteristic, READ_PAIRING_REQUIRED, WRITE_PAIRING_REQUIRED);
    add_characteristic_to_vendor_service(
        &button_ble_service, &back_characteristic);
    add_write_request_handler(&back_characteristic, back_write_handler);
    set_characteristic_value(&back_characteristic, &init_value);
}
