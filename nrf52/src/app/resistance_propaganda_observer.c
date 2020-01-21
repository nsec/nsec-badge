//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)
#if 0
#include "resistance_propaganda_observer.h"
#include "ble/abstract_ble_observer.h"
#include "app/gfx_effect.h"
#include "application.h"
#include "resistance_slideshow.h"
#include "timer.h"

#include <nrf_ble_scan.h>
#include <string.h>


static void on_advertising_report(const ble_gap_evt_adv_report_t* report);

static void on_scan_timeout(const ble_gap_evt_timeout_t* timeout_event);

static void on_valid_packet_received(const ble_gap_evt_adv_report_t*);

static uint64_t last_adv_evt_timestamp = 0;
static uint8_t adv_evt_received = 0;


static struct BleObserver resistance_propaganda_observer = {
    &on_advertising_report,
    &on_scan_timeout,
};

struct ParsedField {
    uint8_t data_length;
    uint8_t type;
    const uint8_t* data;
    uint8_t full_field_length;
};

static void parse_field(const uint8_t* raw_data, struct ParsedField* parsed_field);
static bool parse_scan_response_data(const uint8_t*, uint8_t, struct ParsedField*, struct ParsedField*, struct ParsedField*);


void init_resistance_propaganda_observer(){

}

struct BleObserver* get_resistance_propaganda_observer(){
    return &resistance_propaganda_observer;
}

static void on_advertising_report(const ble_gap_evt_adv_report_t* report) {
    char message[32];
    if(report->scan_rsp == 0 && report->type == BLE_GAP_ADV_TYPE_ADV_NONCONN_IND){
        struct ParsedField flags;
        struct ParsedField manufacturer_specific_data;
        struct ParsedField complete_local_name;
        bool success = parse_scan_response_data(report->data, report->dlen, &flags, &manufacturer_specific_data,
            &complete_local_name);
        if (success) {
            if (flags.type == BLE_GAP_AD_TYPE_FLAGS &&
                    manufacturer_specific_data.type == BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA &&
                    complete_local_name.type == BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME) {

                uint16_t manufacturer_id = *((uint16_t*)manufacturer_specific_data.data);
                if(manufacturer_id == 0x1234) {
                    on_valid_packet_received(report);
                }
            }
        }
    }
}

static void on_valid_packet_received(const ble_gap_evt_adv_report_t* report){
    uint64_t current_time = get_current_time_millis();
    int8_t rssi_threshold = -67;
    if(current_time - last_adv_evt_timestamp > 5000 || report->rssi < rssi_threshold)
        adv_evt_received = 0;
    else if(report->rssi >= rssi_threshold){
        adv_evt_received++;
        if(adv_evt_received > 5){
            application_set(resistance_slideshow_app);
            adv_evt_received = 0;
        }
    }
    last_adv_evt_timestamp = current_time;
}

static void on_scan_timeout(const ble_gap_evt_timeout_t* timeout_event){

}

static bool parse_scan_response_data(const uint8_t* raw_data, uint8_t data_length, struct ParsedField* flags,
        struct ParsedField* manufacturer_specific_data, struct ParsedField* complete_local_name) {
    uint16_t offset = 0;
    parse_field(raw_data, flags);
    offset = flags->full_field_length;
    if(offset >= data_length)
        return false;
    parse_field(raw_data + offset, manufacturer_specific_data);
    offset += manufacturer_specific_data->full_field_length;
    if(offset >= data_length)
        return false;
    parse_field(raw_data + offset, complete_local_name);
    offset += complete_local_name->full_field_length;
    if(data_length != offset)
        return false;
    return true;
}

/**
 * BLE advertising data is separated into fields, with the following format:
 *     length of the field, excluding this byte, but including the type (1 byte)
 *     type of the field (1 byte)
 *     data for the field (length - 1 bytes)
 */
static void parse_field(const uint8_t* raw_data, struct ParsedField* parsed_field){
    parsed_field->data_length = raw_data[0] - sizeof(parsed_field->type);
    parsed_field->type = raw_data[1];
    parsed_field->data = raw_data + 2;
    parsed_field->full_field_length = raw_data[0] + sizeof(raw_data[0]);
}

#endif
