//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "identity.h"
#include <stdint.h>
#include <stdio.h>
#include <nrf52.h>
#include <app_timer.h>
#include "ble/nsec_ble.h"
#include "nsec_nearby_badges.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "3d.h"
#include "persistency.h"

#include "ble/ble_device.h"
#include "ble/service_characteristic.h"
#include "ble/vendor_service.h"
#include "images/default_avatar_bitmap.h"
#include "images/nsec_logo_tiny_bitmap.h"
#include "images/star_bitmap.h"

#define NAME_MAX_LEN 16
#ifdef NSEC_FLAVOR_CTF
#define NAME_CHECK_FLAG "FLAG-badStrcmp:("
#endif

static uint16_t on_name_write(CharacteristicWriteEvent* event);
static uint16_t on_name_read(CharacteristicReadEvent* event);

static char badge_name[NAME_MAX_LEN + 1];
static bool flag_in_name = false;
static bool erase_flag_on_next_read = false;

static struct VendorService identity_ble_service;
static struct ServiceCharacteristic name_characteristic;

static uint16_t service_uuid = 0x001D;   //ID
static uint16_t name_char_uuid = 0x011D; //characteristic 1 of ID, bytes are reversed
static const char name_description[] = "Citizen name, printable-ascii only";

void init_identity_service() {
    memset(badge_name, 0, sizeof(badge_name));

    load_stored_identity(badge_name);

    create_vendor_service(&identity_ble_service, service_uuid);
    add_vendor_service(&identity_ble_service);

    create_characteristic(&name_characteristic, NAME_MAX_LEN, REQUEST_READ, AUTH_WRITE_REQUEST, name_char_uuid);
    set_characteristic_permission(&name_characteristic, READ_OPEN, WRITE_PAIRING_REQUIRED);
    name_characteristic.user_descriptor = name_description;
    name_characteristic.data_type = BLE_GATT_CPF_FORMAT_UTF8S;
    add_characteristic_to_vendor_service(&identity_ble_service, &name_characteristic);
    add_write_request_handler(&name_characteristic, on_name_write);
    add_read_request_handler(&name_characteristic, on_name_read);
    set_characteristic_value(&name_characteristic, (uint8_t*)badge_name);
}

struct VendorService* nsec_identity_get_service(){
    return &identity_ble_service;
}

static bool is_printable_ascii(char c){
    return (c >= 0x20 && c <= 0x7E);
}

/**
 * Check that the string contains only printable ascii characters. It is flawed because it stops at the first null
 * character, giving a flag.
 */
static bool is_name_valid(const char* name){
    for(int i = 0; i < NAME_MAX_LEN; i++) {
        char c = name[i];
        if (c == '\0')
            return true;
        else if (!is_printable_ascii(c))
            return false;
    }
    return true;
}
#ifdef NSEC_FLAVOR_CTF
/**
 * show the flag if the name verification was successfully exploited
 */
static bool show_flag() {
    bool exploited = false;
    for(int i = 0; i < NAME_MAX_LEN; i++) {
        char c = badge_name[i];
        if (!is_printable_ascii(c) && c != '\0') {
            exploited = true;
            break;
        }
    }
    return exploited;
}
#endif

static uint16_t on_name_write(CharacteristicWriteEvent* event){
    if (is_name_valid((const char*)event->data_buffer)){
        memset(badge_name, 0, NAME_MAX_LEN);
        memcpy(badge_name, event->data_buffer, event->data_length);
        update_identity(badge_name);
#ifdef NSEC_FLAVOR_CTF
        if(show_flag()){
            event->data_buffer = (uint8_t*)NAME_CHECK_FLAG;
            event->data_length = NAME_MAX_LEN;
            flag_in_name = true;
            return BLE_GATT_STATUS_SUCCESS;
        }
#endif
        // erase the end of the old name in the characteristic (e.g. writing "1" over "ABC" would become "1BC" without this).
        event->data_buffer = (uint8_t*)badge_name;
        event->data_length = NAME_MAX_LEN;
        return BLE_GATT_STATUS_SUCCESS;
    }
    else {
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }
}

static uint16_t on_name_read(CharacteristicReadEvent* event){
    if(flag_in_name){
        if(!erase_flag_on_next_read) {
            erase_flag_on_next_read = true;
        }
        else{
            set_characteristic_value(&name_characteristic, (uint8_t*)badge_name);
            flag_in_name = false;
            erase_flag_on_next_read = false;
        }
    }
    return BLE_GATT_STATUS_SUCCESS;
}
