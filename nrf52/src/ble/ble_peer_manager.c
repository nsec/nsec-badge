/*
 * Copyright 2019 Nicolas Aubry <nicolas.aubry@hotmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "ble_peer_manager.h"
#include <app_error.h>
#include <peer_manager.h>
#include <ble/peer_manager/peer_manager_types.h>
#include "app/logs.h"
#include "app/pairing_menu.h"
#include "drivers/cli_uart.h"


static void set_security_params(ble_gap_sec_params_t*);

static void peer_event_handler(pm_evt_t const* peer_event){
    switch(peer_event->evt_id){
        case PM_EVT_CONN_SEC_START: //pairing process started
            NRF_LOG_INFO("Connection started\r\n");
            break;
        case PM_EVT_CONN_SEC_SUCCEEDED: //link encrypted
            nsec_ble_hide_pairing_menu();
        case PM_EVT_BONDED_PEER_CONNECTED: // connected with a bonded peer
            NRF_LOG_INFO("connected with a bonded peer\r\n");
            break;
        case PM_EVT_CONN_SEC_FAILED:
            NRF_LOG_INFO("Secure connection failed\r\n");
            nsec_ble_hide_pairing_menu();
            break;
        case PM_EVT_CONN_SEC_PARAMS_REQ:
            NRF_LOG_INFO("connection security params requested\r\n");
            break;
        case PM_EVT_CONN_SEC_CONFIG_REQ: // event for bonding
        {
            NRF_LOG_INFO("connection security config requested\r\n");
            pm_conn_sec_config_t config;
            config.allow_repairing = true;
            pm_conn_sec_config_reply(peer_event->conn_handle, &config);
        }
        default:
            NRF_LOG_INFO("received event 0x%x\r\n", peer_event->evt_id);
            break;
    }
}

void init_peer_manager(){
    APP_ERROR_CHECK(pm_init());
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
