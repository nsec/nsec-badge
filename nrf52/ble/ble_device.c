//
//  ble_device.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2015-11-22.
//
//

#include "nsec_ble_internal.h"
#include <ble_conn_params.h>
/*
#include <ble_bas.h>
#include <ble_hci.h>*/
#include <ble_advdata.h>
//#include <ble_dis.h>
#include <app_timer.h>
#include <peer_manager.h>
#include <app_scheduler.h>

#include "../boards.h"
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include "../led_effects.h"
#include "../logs.h"
#include "nrf_log.h"

#define APP_BLE_OBSERVER_PRIO 3
#define PEER_ADDRESS_SIZE 6


static nrf_sdh_ble_evt_handler_t _nsec_ble_event_handlers[NSEC_BLE_LIMIT_MAX_EVENT_HANDLER];
static nsec_ble_adv_uuid_provider _nsec_ble_adv_uuid_providers[NSEC_BLE_LIMIT_MAX_UUID_PROVIDER];
static uint8_t _nsec_ble_is_enabled = 0;
static nsec_ble_found_nsec_badge_callback _nsec_ble_scan_callback = NULL;

static void _nsec_ble_advertising_start(void);
static void nsec_ble_scan_start(void);


static void ble_event_handler(ble_evt_t const * p_ble_evt, void * p_context){
    //pm_on_ble_evt(p_ble_evt);
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_EVENT_HANDLER; i++) {
        if(_nsec_ble_event_handlers[i] != NULL) {
            _nsec_ble_event_handlers[i](p_ble_evt, p_context);
        }
    }
    switch (p_ble_evt->header.evt_id){
        case BLE_GAP_EVT_CONNECTED: {
            uint8_t * addr = p_ble_evt->evt.gap_evt.params.connected.peer_addr.addr;
            char address[PEER_ADDRESS_SIZE + 1];
            memcpy(address, addr, PEER_ADDRESS_SIZE);
            address[PEER_ADDRESS_SIZE] = 0;
            NRF_LOG_INFO("Connected to %x", address);
            }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            log_status_code("Disconnected. Reason: %d", p_ble_evt->evt.gap_evt.params.disconnected.reason);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            const uint16_t conn = p_ble_evt->evt.gatts_evt.conn_handle;
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(conn, NULL, 0, 0));
            }
            break;

        case BLE_GAP_EVT_ADV_REPORT: {
            if(_nsec_ble_scan_callback == NULL) {
                break;
            }
            ble_gap_evt_adv_report_t * rp = &p_ble_evt->evt.gap_evt.params.adv_report;
            int8_t i = 0;
            while((rp->dlen - i) >= 2) {
                const uint8_t len = rp->data[i++] - 1; // The type is included in the length
                const uint8_t type = rp->data[i++];
                const uint8_t * data = &rp->data[i];
                i += len;

                if(type == BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME) {
                    if(len == 8 && data[0] == 'N' && data[1] == 'S' && data[2] == 'E' && data[3] == 'C') {
                        uint16_t other_id = 0;
                        if(sscanf((const char *) &data[4], "%04hx", &other_id) == 1) {
                            _nsec_ble_scan_callback(other_id, rp->peer_addr.addr, rp->rssi);
                        }
                    }
                }
            }
            }
            break;
    }
}

static void _nsec_pm_evt_handler(pm_evt_t const * event) {

}

static void _nsec_ble_softdevice_init() {
    uint32_t ram_start = 0;
    nrf_sdh_ble_default_cfg_set(BLE_COMMON_CFG_VS_UUID, &ram_start);

#if DEBUG_PRINT_RAM_USAGE
    uint32_t ram_start_copy = 0;
    nrf_sdh_ble_enable(&ram_start_copy);
    //TO DO
#else
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
#endif
    // Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_event_handler, NULL);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void _nsec_ble_gap_params_init(char * device_name)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)device_name, strlen(device_name));
    log_error_code("sd_ble_gap_device_name_set", err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    log_error_code("sd_ble_gap_appearance_set", err_code);

    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(400, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(650, UNIT_1_25_MS);
    gap_conn_params.slave_latency     = 0;
    gap_conn_params.conn_sup_timeout  = MSEC_TO_UNITS(4000, UNIT_10_MS);

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    log_error_code("sd_ble_gap_ppcp_set", err_code);
}

static void _nsec_ble_advertising_init(void)
{
    ble_advdata_t advdata;

    // Build advertising data struct to pass into ble_advdata_set().
    memset(&advdata, 0, sizeof(advdata));

    ble_uuid_t adv_uuids[16];
    int uuid_count = 0;

    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_UUID_PROVIDER && uuid_count < 16; i++) {
        if(_nsec_ble_adv_uuid_providers[i] != NULL) {
            size_t count = 4;
            ble_uuid_t uuids[count];
            _nsec_ble_adv_uuid_providers[i](&count, uuids);
            for(int j = 0; j < count && uuid_count < 16; j++) {
                adv_uuids[uuid_count++] = uuids[j];
            }
        }
    }

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = uuid_count;
    advdata.uuids_complete.p_uuids  = adv_uuids;

    log_error_code("ble_advdata_set", ble_advdata_set(&advdata, NULL));
}

static void nsec_ble_disable_task(void * context, uint16_t size) {
    sd_ble_gap_scan_stop();
    sd_ble_gap_adv_stop();
}

static void nsec_ble_enable_task(void * context, uint16_t size) {
    _nsec_ble_advertising_start();
    nsec_ble_scan_start();
}

uint8_t nsec_ble_toggle(void) {
    if(_nsec_ble_is_enabled) {
        app_sched_event_put(NULL, 0, nsec_ble_disable_task);
        _nsec_ble_is_enabled = 0;
    }
    else {
        app_sched_event_put(NULL, 0, nsec_ble_enable_task);
        _nsec_ble_is_enabled = 1;
    }
    return _nsec_ble_is_enabled;
}

static void _nsec_ble_advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;

    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = MSEC_TO_UNITS(1216,UNIT_0_625_MS);
    adv_params.timeout     = 0;

    err_code = sd_ble_gap_adv_start(&adv_params, BLE_COMMON_CFG_VS_UUID);
    log_error_code("sd_ble_gap_adv_start", err_code);
    APP_ERROR_CHECK(err_code);
}

void nsec_ble_register_evt_handler(nrf_sdh_ble_evt_handler_t handler) {
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_EVENT_HANDLER; i++) {
        if(_nsec_ble_event_handlers[i] == NULL) {
            _nsec_ble_event_handlers[i] = handler;
            break;
        }
    }
}

void nsec_ble_set_scan_callback(nsec_ble_found_nsec_badge_callback callback) {
    _nsec_ble_scan_callback = callback;
}

void nsec_ble_register_adv_uuid_provider(nsec_ble_adv_uuid_provider provider) {
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_UUID_PROVIDER; i++) {
        if(_nsec_ble_adv_uuid_providers[i] == NULL) {
            _nsec_ble_adv_uuid_providers[i] = provider;
            break;
        }
    }
    sd_ble_gap_adv_stop();
    _nsec_ble_advertising_init();
    _nsec_ble_advertising_start();
}

static void nsec_ble_scan_start(void) {
    ble_gap_scan_params_t scan_params;
    scan_params.active = 0;
    scan_params.adv_dir_report = 0;
    scan_params.use_whitelist = 0;
    scan_params.timeout = 0;
    scan_params.window = MSEC_TO_UNITS(40, UNIT_0_625_MS);
    scan_params.interval = MSEC_TO_UNITS(240, UNIT_0_625_MS);

    log_error_code("sd_ble_gap_scan_start", sd_ble_gap_scan_start(&scan_params));
}

int nsec_ble_init(char * device_name) {
    _nsec_ble_softdevice_init();

    //APP_ERROR_CHECK(softdevice_ble_evt_handler_set(_nsec_ble_evt_dispatch));

    _nsec_ble_gap_params_init(device_name);

    pm_init();
    ble_gap_sec_params_t sec_params;
    bzero(&sec_params, sizeof(sec_params));

    sec_params.bond = 1;
    sec_params.mitm = 0;
    sec_params.lesc = 0;
    sec_params.keypress = 0;
    sec_params.io_caps = BLE_GAP_IO_CAPS_NONE;
    sec_params.oob = 0;
    sec_params.min_key_size = 7;
    sec_params.max_key_size = 16;
    sec_params.kdist_own.enc = 1;
    sec_params.kdist_own.id = 1;
    sec_params.kdist_peer.enc = 1;
    sec_params.kdist_peer.id = 1;

    pm_sec_params_set(&sec_params);

    pm_register(_nsec_pm_evt_handler);

    bzero(_nsec_ble_event_handlers, sizeof(_nsec_ble_event_handlers));
    bzero(_nsec_ble_adv_uuid_providers, sizeof(_nsec_ble_adv_uuid_providers));

    _nsec_ble_advertising_init();
    _nsec_ble_advertising_start();
    nsec_ble_scan_start();
    _nsec_ble_is_enabled = 1;

    return NRF_SUCCESS;
}
