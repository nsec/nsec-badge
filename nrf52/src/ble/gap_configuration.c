//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "gap_configuration.h"
#include "app/logs.h"

#include <ble_gap.h>
#include <peer_manager.h>
#include <ble_advdata.h>
#include <ble_conn_params.h>


static void set_default_advertising_data(ble_uuid_t*);
static void set_default_security_parameters();
static void set_default_scan_parameters();
static void set_default_advertising_parameters(ble_gap_adv_params_t* advertising_parameters);
static void set_default_connection_parameters(const char * device_name);


void set_default_gap_parameters(const char * device_name, ble_gap_adv_params_t* advertising_parameters){
    set_default_advertising_parameters(advertising_parameters);
    set_default_security_parameters();
    set_default_scan_parameters();
    set_default_connection_parameters(device_name);
}

void set_default_advertised_service(VendorService* service){
    set_default_advertising_data(&service->uuid);
}

static void set_default_security_parameters(){
    ble_gap_sec_params_t security_parameters;
    security_parameters.bond = 1;
    security_parameters.mitm = 0;
    security_parameters.lesc = 0; //LE secure connection.
    security_parameters.keypress = 0;
    security_parameters.io_caps = BLE_GAP_IO_CAPS_NONE;
    security_parameters.oob = 0;
    security_parameters.min_key_size = 7;
    security_parameters.max_key_size = 16;
    security_parameters.kdist_own.enc = 1;
    security_parameters.kdist_own.id = 1;
    security_parameters.kdist_own.sign = 0;
    security_parameters.kdist_own.link = 0;
    security_parameters.kdist_peer.enc = 1;
    security_parameters.kdist_peer.id = 1;
    security_parameters.kdist_peer.sign = 0;
    security_parameters.kdist_peer.link = 0;
    pm_sec_params_set(&security_parameters);
}

static void set_default_scan_parameters(){
    ble_gap_scan_params_t scan_parameters;
    scan_parameters.active = 0;
    scan_parameters.adv_dir_report = 0;
    scan_parameters.use_whitelist = 0;
    scan_parameters.timeout = 0;
    scan_parameters.window = MSEC_TO_UNITS(40, UNIT_0_625_MS);
    scan_parameters.interval = MSEC_TO_UNITS(240, UNIT_0_625_MS);
    sd_ble_gap_scan_start(&scan_parameters);
}

static void set_default_advertising_parameters(ble_gap_adv_params_t* advertising_parameters){
    advertising_parameters->type = BLE_GAP_ADV_TYPE_ADV_IND;
    advertising_parameters->p_peer_addr = NULL;
    advertising_parameters->fp = BLE_GAP_ADV_FP_ANY;
    advertising_parameters->interval = MSEC_TO_UNITS(1216,UNIT_0_625_MS);
    advertising_parameters->timeout = 0;
    advertising_parameters->channel_mask.ch_37_off = 0;
    advertising_parameters->channel_mask.ch_38_off = 0;
    advertising_parameters->channel_mask.ch_39_off = 0;
}

static void set_default_connection_parameters(const char * device_name){
    uint32_t error_code;
    ble_gap_conn_params_t gap_connection_parameters;
    ble_gap_conn_sec_mode_t security_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&security_mode);

    error_code = sd_ble_gap_device_name_set(&security_mode, (const uint8_t *)device_name, strlen(device_name));
    log_error_code("sd_ble_gap_device_name_set", error_code);

    error_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    log_error_code("sd_ble_gap_appearance_set", error_code);

    gap_connection_parameters.min_conn_interval = MSEC_TO_UNITS(400, UNIT_1_25_MS);
    gap_connection_parameters.max_conn_interval = MSEC_TO_UNITS(650, UNIT_1_25_MS);
    gap_connection_parameters.slave_latency = 0;
    gap_connection_parameters.conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS);

    error_code = sd_ble_gap_ppcp_set(&gap_connection_parameters);
    log_error_code("sd_ble_gap_ppcp_set", error_code);
}

static void set_default_advertising_data(ble_uuid_t* uuid){
    ble_advdata_t advdata;

    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = 1;
    advdata.uuids_complete.p_uuids  = uuid;

    log_error_code("ble_advdata_set", ble_advdata_set(&advdata, NULL));
}
