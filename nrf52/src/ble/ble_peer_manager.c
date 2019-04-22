//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "ble_peer_manager.h"
#include <app_error.h>
#include <peer_manager.h>
#include <ble/peer_manager/peer_manager_types.h>
#include "drivers/uart.h"
#include "drivers/led_effects.h"
#include "app/pairing_menu.h"


static uint32_t reply_to_pairing_request(uint16_t connection_handle, void const* request_context);
static void set_security_params(ble_gap_sec_params_t*);

static void peer_event_handler(pm_evt_t const* peer_event){
    switch(peer_event->evt_id){
        case PM_EVT_CONN_SEC_START: //pairing process started
            break;
        case PM_EVT_CONN_SEC_SUCCEEDED: //link encrypted
            break;
        case PM_EVT_BONDED_PEER_CONNECTED: // connected with a bonded peer
            break;
        case PM_EVT_CONN_SEC_FAILED:
            break;
        case PM_EVT_CONN_SEC_PARAMS_REQ:
            break;
        case PM_EVT_CONN_SEC_CONFIG_REQ: // event for bonding
        {
            pm_conn_sec_config_t config;
            config.allow_repairing = true;
            pm_conn_sec_config_reply(peer_event->conn_handle, &config);
        }
        default:
        {
            /*uint16_t event = peer_event->evt_id;
            char base_message[] = "Event 0x%x received";
            char full_message[sizeof(base_message) + 3];
            snprintf(full_message, sizeof(full_message), base_message, event);
            display_message(full_message);*/
        }
    }
    nsec_neoPixel_show();
}

void init_peer_manager(){
    APP_ERROR_CHECK(pm_init());
    APP_ERROR_CHECK(pm_peers_delete()); // TODO need to fix this.
    APP_ERROR_CHECK(pm_register(peer_event_handler));
    ble_gap_sec_params_t sec_params;
    set_security_params(&sec_params);
    APP_ERROR_CHECK(pm_sec_params_set(&sec_params));
}

static void set_security_params(ble_gap_sec_params_t* security_parameters){
    memset(security_parameters, 0, sizeof(ble_gap_sec_params_t));
    security_parameters->bond = 1;
    security_parameters->mitm = 1;
    security_parameters->lesc = 0; //LE secure connection, stay in legacy for now.
    security_parameters->keypress = 0;
    security_parameters->io_caps = BLE_GAP_IO_CAPS_DISPLAY_YESNO;
    security_parameters->oob = 0;
    security_parameters->min_key_size = 7;
    security_parameters->max_key_size = 16;
    security_parameters->kdist_own.enc = 1;
    security_parameters->kdist_own.id = 1;
    security_parameters->kdist_own.sign = 0;
    security_parameters->kdist_own.link = 0;
    security_parameters->kdist_peer.enc = 1;
    security_parameters->kdist_peer.id = 1;
    security_parameters->kdist_peer.sign = 0;
    security_parameters->kdist_peer.link = 0;
}
