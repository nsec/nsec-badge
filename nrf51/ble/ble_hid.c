//
//  ble_hid.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-04-27.
//
//

#include "nsec_ble_internal.h"
#include <ble_hids.h>
#include <app_timer.h>

#include "../ssd1306.h"
#include <stdio.h>


static void _nsec_ble_hid_event_dispatch(ble_evt_t * p_evt);
static void _nsec_ble_hid_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt);
static void _nsec_ble_hid_error_handler(uint32_t nrf_error);
static void _nsec_ble_hid_adv_uuid_provider(size_t * count, ble_uuid_t * uuids);
static void _nsec_ble_send_mouse_move(void * p_context);

static ble_hids_t _hid_handle;

struct mouse_event_s {
    uint8_t buttons;
    uint8_t x;
    uint8_t y;
};

static app_timer_id_t _mouse_timer;
static uint32_t _tick_count = 0;

void nsec_ble_hid_add_device(void) {
    ble_hids_init_t init_value;

    bzero(&init_value, sizeof(init_value));
    bzero(&_hid_handle, sizeof(_hid_handle));

    init_value.evt_handler = _nsec_ble_hid_event_handler;
    init_value.error_handler = _nsec_ble_hid_error_handler;
    init_value.is_mouse = 1;

    uint8_t report_map[] = {
        0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
        0x09, 0x02,                    // USAGE (Mouse)
        0xa1, 0x01,                    // COLLECTION (Application)
        0x09, 0x01,                    //   USAGE (Pointer)
        0xa1, 0x00,                    //   COLLECTION (Physical)
        0x05, 0x09,                    //     USAGE_PAGE (Button)
        0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
        0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
        0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
        0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
        0x95, 0x03,                    //     REPORT_COUNT (3)
        0x75, 0x01,                    //     REPORT_SIZE (1)
        0x81, 0x02,                    //     INPUT (Data,Var,Abs)
        0x95, 0x01,                    //     REPORT_COUNT (1)
        0x75, 0x05,                    //     REPORT_SIZE (5)
        0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
        0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
        0x09, 0x30,                    //     USAGE (X)
        0x09, 0x31,                    //     USAGE (Y)
        0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
        0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
        0x75, 0x08,                    //     REPORT_SIZE (8)
        0x95, 0x02,                    //     REPORT_COUNT (2)
        0x81, 0x06,                    //     INPUT (Data,Var,Rel)
        0xc0,                          //   END_COLLECTION
        0xc0                           // END_COLLECTION
    };

    init_value.rep_map.data_len = sizeof(report_map);
    init_value.rep_map.p_data = report_map;

    ble_hids_inp_rep_init_t input_response_init;
    input_response_init.max_len = 4;
    input_response_init.read_resp = 0;
    input_response_init.rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&input_response_init.security_mode.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&input_response_init.security_mode.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&input_response_init.security_mode.cccd_write_perm);

    init_value.p_inp_rep_array = &input_response_init;
    init_value.inp_rep_count = 1;

    init_value.hid_information.bcd_hid = 0x0101;
    init_value.hid_information.flags = HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.rep_map.security_mode.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&init_value.rep_map.security_mode.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.hid_information.security_mode.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&init_value.hid_information.security_mode.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_boot_mouse_inp_rep.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_boot_mouse_inp_rep.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&init_value.security_mode_boot_mouse_inp_rep.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_protocol.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_protocol.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_ctrl_point.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&init_value.security_mode_ctrl_point.write_perm);

    APP_ERROR_CHECK(ble_hids_init(&_hid_handle, &init_value));
    APP_ERROR_CHECK(app_timer_create(&_mouse_timer, APP_TIMER_MODE_REPEATED, _nsec_ble_send_mouse_move));

    nsec_ble_register_evt_handler(_nsec_ble_hid_event_dispatch);
    nsec_ble_register_adv_uuid_provider(_nsec_ble_hid_adv_uuid_provider);
}

static void _nsec_ble_hid_event_dispatch(ble_evt_t * p_evt) {
    ble_hids_on_ble_evt(&_hid_handle, p_evt);
    if(p_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED) {
        APP_ERROR_CHECK(app_timer_stop(_mouse_timer));
        _tick_count = 0;

    }
}

static void _nsec_ble_hid_adv_uuid_provider(size_t * count, ble_uuid_t * uuids) {
    uuids[0].type = BLE_UUID_TYPE_BLE;
    uuids[0].uuid = BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE;
    *count = 1;
}


static void _nsec_ble_hid_event_handler(ble_hids_t * p_hids, ble_hids_evt_t * p_evt) {
    char buf[32];
    gfx_setCursor(0, 0);
    snprintf(buf, sizeof(buf), "Report: %d", p_evt->evt_type);
    gfx_puts(buf);
    gfx_update();

    switch (p_evt->evt_type) {
        case BLE_HIDS_EVT_HOST_SUSP:
        case BLE_HIDS_EVT_HOST_EXIT_SUSP:
            break;
        case BLE_HIDS_EVT_NOTIF_ENABLED:
            _nsec_ble_send_mouse_move(NULL);
            app_timer_start(_mouse_timer, APP_TIMER_TICKS(1000, 0), NULL);
            break;
        case BLE_HIDS_EVT_NOTIF_DISABLED:
        case BLE_HIDS_EVT_REP_CHAR_WRITE:
        case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
        case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
        case BLE_HIDS_EVT_REPORT_READ:
            break;
    }
}

static void _nsec_ble_hid_error_handler(uint32_t nrf_error) {
    APP_ERROR_CHECK(nrf_error);
}

static void _nsec_ble_send_mouse_move(void * p_context) {
    if(_tick_count++ > 200) {
        APP_ERROR_CHECK(app_timer_stop(_mouse_timer));
        _tick_count = 0;
    }
    struct mouse_event_s evt;
    evt.buttons = 0;
    evt.x = 2;
    evt.y = 2;
    APP_ERROR_CHECK(ble_hids_inp_rep_send(&_hid_handle, 0, sizeof(evt), (uint8_t*) &evt));
}
