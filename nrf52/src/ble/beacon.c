//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "beacon.h"
#include "abstract_advertiser.h"
#include "utils.h"

#include <ble_gap.h>
#include <logs.h>


static void start_broadcasting();
static void stop_broadcasting();
static void on_ble_advertising_event(ble_evt_t const * ble_event);
static void configure_advertising_parameters(ble_gap_adv_params_t* adv_params);
static void configure_advertising_data();


struct Advertiser beacon =
    {
        &start_broadcasting,
        &stop_broadcasting,
        &on_ble_advertising_event
    };

static struct BeaconConfig {
    struct AdvertisingName* name;
    ble_advdata_t adv_data;
    ble_advdata_manuf_data_t broadcast_data;
    uint16_t advertising_interval;
} beacon_config;


void set_broadcast_data(uint8_t* data, uint16_t size){
    beacon_config.broadcast_data.company_identifier = 0x1234;
    beacon_config.broadcast_data.data.p_data = data;
    beacon_config.broadcast_data.data.size = size;
}

void set_beacon_name(struct AdvertisingName* name){
    beacon_config.name = name;
}

void set_advertising_interval_ms(uint16_t interval){
    float interval_0_625_ms = interval / 0.625F;
    if(interval_0_625_ms < BLE_GAP_ADV_INTERVAL_MIN)
        interval_0_625_ms = BLE_GAP_ADV_INTERVAL_MIN;
    else if(interval_0_625_ms > BLE_GAP_ADV_INTERVAL_MAX)
        interval_0_625_ms = BLE_GAP_ADV_INTERVAL_MAX;
    beacon_config.advertising_interval = (uint16_t) interval_0_625_ms;
}

void test_advertising(){
    start_broadcasting();
}

static void start_broadcasting(){
    configure_advertising_data();
    ble_gap_adv_params_t adv_params;
    configure_advertising_parameters(&adv_params);

    log_error_code("ble_advdata_set", ble_advdata_set(&beacon_config.adv_data, NULL));
    log_error_code("sd_ble_gap_adv_start", sd_ble_gap_adv_start(&adv_params, 0));
}

static void stop_broadcasting(){
    APP_ERROR_CHECK(sd_ble_gap_adv_stop());
}

static void configure_advertising_data(){
    memset(&beacon_config.adv_data, 0, sizeof(beacon_config.adv_data));

    ble_gap_conn_sec_mode_t security_mode;
    beacon_config.adv_data.name_type = beacon_config.name->name_type;
    if(beacon_config.name->name_type == BLE_ADVDATA_SHORT_NAME)
        beacon_config.adv_data.short_name_len = (uint8_t) max(beacon_config.name->name_length, 8);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&security_mode);
    APP_ERROR_CHECK(sd_ble_gap_device_name_set(&security_mode, beacon_config.name->name, beacon_config.name->name_length));

    beacon_config.adv_data.include_appearance = false;
    beacon_config.adv_data.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    beacon_config.adv_data.p_manuf_specific_data = &beacon_config.broadcast_data;
}

static void configure_advertising_parameters(ble_gap_adv_params_t* adv_params){
    memset(adv_params, 0, sizeof(ble_gap_adv_params_t));
    adv_params->type = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    adv_params->timeout = 0; // no timeout
    adv_params->interval = beacon_config.advertising_interval;
}

static void on_ble_advertising_event(ble_evt_t const * ble_event){

}
