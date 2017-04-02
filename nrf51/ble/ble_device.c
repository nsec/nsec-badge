//
//  ble_device.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2015-11-22.
//
//

#include "nsec_ble_internal.h"
#include <ble_conn_params.h>
#include <ble_bas.h>
#include <ble_hci.h>
#include <ble_advdata.h>
#include <ble_dis.h>
#include <app_timer.h>
#include <peer_manager.h>

#include "../boards.h"
#include <nrf_gpio.h>

static ble_evt_handler_t _nsec_ble_event_handlers[NSEC_BLE_LIMIT_MAX_EVENT_HANDLER];
static nsec_ble_adv_uuid_provider _nsec_ble_adv_uuid_providers[NSEC_BLE_LIMIT_MAX_UUID_PROVIDER];
static uint8_t _nsec_ble_is_enabled = 0;

static void _nsec_ble_evt_dispatch(ble_evt_t * p_ble_evt);
static void _nsec_ble_advertising_start(void);

static void _nsec_ble_evt_dispatch(ble_evt_t * p_ble_evt) {
    pm_on_ble_evt(p_ble_evt);
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_EVENT_HANDLER; i++) {
        if(_nsec_ble_event_handlers[i] != NULL) {
            _nsec_ble_event_handlers[i](p_ble_evt);
        }
    }
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED: {
            //uint8_t * addr = p_ble_evt->evt.gap_evt.params.connected.peer_addr.addr;
            //size_t addr_size = sizeof(p_ble_evt->evt.gap_evt.params.connected.peer_addr.addr);
            //for(int i = 0; i < addr_size; i++) {
            //    char hex[4];
            //    snprintf(hex, sizeof(hex), "%02x", addr[i]);
            //    gfx_puts(hex);
            //}
            //gfx_update();
            nrf_gpio_cfg_output(LED_GREEN);
            nrf_gpio_pin_clear(LED_GREEN);
            }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_cfg_output(LED_GREEN);
            nrf_gpio_pin_set(LED_GREEN);
            _nsec_ble_advertising_start();
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            const uint16_t conn = p_ble_evt->evt.gatts_evt.conn_handle;
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(conn, NULL, 0, 0));
            }
            break;
    }
}

static void _nsec_pm_evt_handler(pm_evt_t const * event) {

}

static void _nsec_ble_softdevice_init() {
    ble_enable_params_t ble_enable_params;
    extern uint32_t __data_start__;
    volatile uint32_t ram_start = (uint32_t) &__data_start__;
    uint32_t ram_start_copy = ram_start;

    softdevice_enable_get_default_config(0, 1, &ble_enable_params);

    ble_enable_params.common_enable_params.vs_uuid_count = 2;
#if DEBUG_PRINT_RAM_USAGE
    ram_start_copy = 0;
    sd_ble_enable(&ble_enable_params, &ram_start_copy);
    APP_ERROR_CHECK(ram_start_copy - ram_start);
#else
    APP_ERROR_CHECK(sd_ble_enable(&ble_enable_params, &ram_start_copy));
#endif
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

    ble_srv_utf8_str_t device_id_utf8_str;
    ble_srv_ascii_to_utf8(&device_id_utf8_str, device_name);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          device_id_utf8_str.p_str,
                                          device_id_utf8_str.length);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MSEC_TO_UNITS(400, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval = MSEC_TO_UNITS(650, UNIT_1_25_MS);
    gap_conn_params.slave_latency     = 0;
    gap_conn_params.conn_sup_timeout  = MSEC_TO_UNITS(4000, UNIT_10_MS);

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
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

    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.include_appearance      = false;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = uuid_count;
    advdata.uuids_complete.p_uuids  = adv_uuids;

    APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));
}

uint8_t nsec_ble_toggle(void) {
    if(_nsec_ble_is_enabled) {
        sd_ble_gap_adv_stop();
        _nsec_ble_is_enabled = 0;
    }
    else {
        _nsec_ble_advertising_start();
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
    adv_params.p_whitelist = NULL;
    adv_params.interval    = MSEC_TO_UNITS(1216,UNIT_0_625_MS);
    adv_params.timeout     = 0;

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);
}

void nsec_ble_register_evt_handler(ble_evt_handler_t handler) {
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_EVENT_HANDLER; i++) {
        if(_nsec_ble_event_handlers[i] == NULL) {
            _nsec_ble_event_handlers[i] = handler;
            break;
        }
    }
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

int nsec_ble_init(char * device_name) {
    _nsec_ble_softdevice_init();

    APP_ERROR_CHECK(softdevice_ble_evt_handler_set(_nsec_ble_evt_dispatch));

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

    //bzero(_nsec_ble_vendor_services, sizeof(_nsec_ble_vendor_services));
    //bzero(_nsec_ble_vendor_services_characteristics, sizeof(_nsec_ble_vendor_services_characteristics));
    bzero(_nsec_ble_event_handlers, sizeof(_nsec_ble_event_handlers));
    bzero(_nsec_ble_adv_uuid_providers, sizeof(_nsec_ble_adv_uuid_providers));

    _nsec_ble_advertising_init();
    _nsec_ble_advertising_start();
    _nsec_ble_is_enabled = 1;

    return NRF_SUCCESS;
}
