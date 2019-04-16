//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "abstract_advertiser.h"
#include "service_advertiser.h"
#include "ble_device.h"

#include <app_error.h>
#include <ble_advertising.h>


static ble_advertising_init_t advertising_params;
static ble_advertising_t advertising_module;
static void start_advertising();
static void stop_advertising();
static void error_handler(uint32_t nrf_error);
static void event_handler(ble_adv_evt_t const adv_evt);
static void on_ble_advertising_event(ble_evt_t const * ble_event);


static struct Advertiser service_advertiser =
    {
        &start_advertising,
        &stop_advertising,
        &on_ble_advertising_event
    };

void init_adv_module(){
    memset(&advertising_params, 0, sizeof(advertising_params));

    advertising_params.advdata.include_appearance = false;
    advertising_params.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
}

void enable_fast_advertising_mode(uint32_t timeout){
    advertising_params.config.ble_adv_fast_enabled = true;
    advertising_params.config.ble_adv_fast_timeout = timeout;
    advertising_params.config.ble_adv_fast_interval = 50; // ms?
}

void enable_slow_advertising_mode(uint32_t timeout){
    advertising_params.config.ble_adv_slow_enabled = true;
    advertising_params.config.ble_adv_slow_timeout = timeout;
    advertising_params.config.ble_adv_slow_interval = 500; // ms?
}

void set_device_name(const char* name){
    const uint8_t* device_name = (const uint8_t *)name;
    ble_gap_conn_sec_mode_t security_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&security_mode);
    // Always use full name, as the encoding lib will use the appropriate name type in the end.
    advertising_params.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    size_t name_length = strlen(name);
    APP_ERROR_CHECK(sd_ble_gap_device_name_set(&security_mode, device_name, (uint16_t)name_length));
}

void set_vendor_service_in_advertising_packet(struct VendorService* service, bool more_available){
    if(more_available){
        advertising_params.advdata.uuids_more_available.uuid_cnt = 1;
        advertising_params.advdata.uuids_more_available.p_uuids = &service->uuid;
    }
    else{
        advertising_params.advdata.uuids_complete.uuid_cnt = 1;
        advertising_params.advdata.uuids_complete.p_uuids = &service->uuid;
    }
}

void set_vendor_service_in_scan_response(struct VendorService* service, bool more_available){
    if(more_available){
        advertising_params.advdata.uuids_more_available.uuid_cnt = 1;
        advertising_params.advdata.uuids_more_available.p_uuids = &service->uuid;
    }
    else{
        advertising_params.srdata.uuids_complete.uuid_cnt = 1;
        advertising_params.srdata.uuids_complete.p_uuids = &service->uuid;
    }
}

struct Advertiser* get_service_advertiser(){
    return &service_advertiser;
}

static void start_advertising(){
    advertising_params.error_handler = error_handler;
    advertising_params.evt_handler = event_handler;

    APP_ERROR_CHECK(ble_advertising_init(&advertising_module, &advertising_params));
    APP_ERROR_CHECK(ble_advertising_start(&advertising_module, BLE_ADV_MODE_FAST));
}

static void stop_advertising(){

}

static void error_handler(uint32_t nrf_error){
    //log_error_code("advertising", nrf_error);
}

static void event_handler(ble_adv_evt_t const adv_evt){
    //NRF_LOG_WARNING("advertising event %d", adv_evt);
}

static void on_ble_advertising_event(ble_evt_t const * ble_event){
    ble_advertising_on_ble_evt(ble_event, &advertising_module);
}
