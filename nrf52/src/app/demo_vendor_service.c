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
#include <app_timer.h>
#include <ble/uuid.h>

static struct VendorService demo_service;
static struct ServiceCharacteristic led0;
static struct ServiceCharacteristic led1;
static struct ServiceCharacteristic led2;
static struct ServiceCharacteristic notify_char;
static uint16_t demo_service_uuid = 0xDE30;
static uint16_t led0_uuid = 0xDE31;
static uint16_t led1_uuid = 0xDE32;
static uint16_t led2_uuid = 0xDE33;
static uint16_t notify_char_uuid = 0xDE34;

static uint8_t led_write_request_no_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B
static uint8_t led_write_request_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B
static uint8_t led_read_request_auth_color[3] = {0x0, 0x0, 0x0}; // [0]: R, [1]: G, [2]: B
static uint8_t notify_char_data = 0;

APP_TIMER_DEF(m_notify_timer);

static bool _update_char;

void update_char(void)
{
    if(_update_char){
        _update_char = false;
        notify_char_data++;
        set_characteristic_value(&notify_char, &notify_char_data);
    }
}

//static void on_led_write_command_color_written(CharacteristicWriteEvent* event){
//    if(event->data_length <= sizeof(led_write_request_no_auth_color)) {
//        memcpy(led_write_request_no_auth_color, event->data_buffer, event->data_length);
//        nsec_neoPixel_set_pixel_color(0, led_write_request_no_auth_color[0], led_write_request_no_auth_color[1],
//                                      led_write_request_no_auth_color[2]);
//        nsec_neoPixel_show();
//    }
//}

static uint16_t on_led_write_request(CharacteristicWriteEvent* event){
    if(event->data_length <= 3) {
        uint16_t led_number = event->uuid - demo_service_uuid - 1;
        nsec_neoPixel_set_pixel_color(led_number, event->data_buffer[0], event->data_buffer[1], event->data_buffer[2]);
        nsec_neoPixel_show();
        return BLE_GATT_STATUS_SUCCESS;
    }
    return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
}

static uint16_t on_led_2_read_request(CharacteristicReadEvent* event) {
    if(led_read_request_auth_color[1] == 255){
        return BLE_GATT_STATUS_SUCCESS;
    }
    return BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
}

//static void on_led_2_write_done(CharacteristicWriteEvent* event){
//    if(event->data_length <= sizeof(led_read_request_auth_color)) {
//        memcpy(led_read_request_auth_color, event->data_buffer, event->data_length);
//        nsec_neoPixel_set_pixel_color(2, led_read_request_auth_color[0], led_read_request_auth_color[1],
//                                      led_read_request_auth_color[2]);
//        nsec_neoPixel_show();
//    }
//}

static void on_timeout(void * context){
    _update_char = true;
}

void nsec_init_demo_vendor_service(void)
{
    ble_uuid_t uuid = {.uuid = demo_service_uuid, .type = TYPE_NSEC_UUID};
    create_vendor_service(&demo_service, &uuid);
    add_vendor_service(&demo_service);

    create_characteristic(&led0, 3, AUTO_READ, AUTH_WRITE_REQUEST, led0_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led0);
    add_write_request_handler(&led0, on_led_write_request);
    set_characteristic_value(&led0, led_write_request_no_auth_color);

    create_characteristic(&led1, 3, AUTO_READ, AUTH_WRITE_REQUEST, led1_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led1);
    add_write_request_handler(&led1, on_led_write_request);
    set_characteristic_value(&led1, led_write_request_auth_color);

    create_characteristic(&led2, 3, REQUEST_READ, AUTH_WRITE_REQUEST, led2_uuid);
    add_characteristic_to_vendor_service(&demo_service, &led2);
    add_write_request_handler(&led2, on_led_write_request);
    add_read_request_handler(&led2, on_led_2_read_request);
    set_characteristic_value(&led2, led_read_request_auth_color);

    create_characteristic(&notify_char, sizeof(notify_char_data), AUTO_READ, DENY_WRITE, notify_char_uuid);
    notify_char.allow_notify = true;
    add_characteristic_to_vendor_service(&demo_service, &notify_char);
    set_characteristic_value(&notify_char, &notify_char_data);

    ret_code_t err_code = app_timer_create(&m_notify_timer, APP_TIMER_MODE_REPEATED, on_timeout);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_notify_timer, APP_TIMER_TICKS(5000), NULL);
    APP_ERROR_CHECK(err_code);
}

struct VendorService *nsec_get_demo_service(void)
{
    return &demo_service;
}
