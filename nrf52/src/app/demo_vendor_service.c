//
// Created by nicolas on 4/18/19.
//

#include "demo_vendor_service.h"
#include "ble/vendor_service.h"
#include "ble/service_characteristic.h"
#include "ble/ble_device.h"
#include "drivers/led_effects.h"

#include <string.h>
#include <app_error.h>

static struct VendorService demo_service;
static struct ServiceCharacteristic led_write_request_no_auth_char;
static struct ServiceCharacteristic led_write_request_auth_char;
static struct ServiceCharacteristic led_read_request_auth_char;
static uint16_t demo_service_uuid = 0xDE30;
static uint16_t led_write_request_no_auth_char_uuid = 0xDE31;
static uint16_t led_write_request_auth_char_uuid = 0xDE32;
static uint16_t led_read_request_auth_char_uuid = 0xDE33;

static uint8_t led_write_request_no_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B
static uint8_t led_write_request_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B
static uint8_t led_read_request_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B


static void on_led_write_command_color_written(CharacteristicWriteEvent* event){
    if(event->data_length <= sizeof(led_write_request_no_auth_color)) {
        memcpy(led_write_request_no_auth_color, event->data_buffer, event->data_length);
        nsec_neoPixel_set_pixel_color(0, led_write_request_no_auth_color[0], led_write_request_no_auth_color[1],
                                      led_write_request_no_auth_color[2]);
        nsec_neoPixel_show();
    }
}

static uint16_t on_led_write_request(CharacteristicWriteEvent* event){
    if(event->data_length <= sizeof(led_write_request_auth_color)) {
        if(event->data_buffer[0] == 0) {
            memcpy(led_write_request_auth_color, event->data_buffer, event->data_length);
            nsec_neoPixel_set_pixel_color(1, led_write_request_auth_color[0], led_write_request_auth_color[1],
                                          led_write_request_auth_color[2]);
            nsec_neoPixel_show();
            return BLE_GATT_STATUS_SUCCESS;
        }
    }
    return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
}

static uint16_t on_led_2_read_request(CharacteristicReadEvent* event) {
    if(led_read_request_auth_color[1] == 255){
        return BLE_GATT_STATUS_SUCCESS;
    }
    return BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
}

static void on_led_2_write_done(CharacteristicWriteEvent* event){
    if(event->data_length <= sizeof(led_read_request_auth_color)) {
        memcpy(led_read_request_auth_color, event->data_buffer, event->data_length);
        nsec_neoPixel_set_pixel_color(2, led_read_request_auth_color[0], led_read_request_auth_color[1],
                                      led_read_request_auth_color[2]);
        nsec_neoPixel_show();
    }
}

void nsec_init_demo_vendor_service(){
    create_vendor_service(&demo_service, demo_service_uuid);
    add_vendor_service(&demo_service);

    create_characteristic(&led_write_request_no_auth_char, sizeof(led_write_request_no_auth_color), AUTO_READ,
                          WRITE_REQUEST, led_write_request_no_auth_char_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led_write_request_no_auth_char);
    add_write_operation_done_handler(&led_write_request_no_auth_char, on_led_write_command_color_written);
    set_characteristic_value(&led_write_request_no_auth_char, led_write_request_no_auth_color);

    create_characteristic(&led_write_request_auth_char, sizeof(led_write_request_auth_color), AUTO_READ,
                          AUTH_WRITE_REQUEST, led_write_request_auth_char_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led_write_request_auth_char);
    add_write_request_handler(&led_write_request_auth_char, on_led_write_request);
    set_characteristic_value(&led_write_request_auth_char, led_write_request_auth_color);

    create_characteristic(&led_read_request_auth_char, sizeof(led_read_request_auth_color), REQUEST_READ,
                          WRITE_REQUEST, led_read_request_auth_char_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led_read_request_auth_char);
    add_write_operation_done_handler(&led_read_request_auth_char, on_led_2_write_done);
    add_read_request_handler(&led_read_request_auth_char, on_led_2_read_request);
    set_characteristic_value(&led_read_request_auth_char, led_read_request_auth_color);
}

struct VendorService* nsec_get_demo_service(){
    return &demo_service;
}
