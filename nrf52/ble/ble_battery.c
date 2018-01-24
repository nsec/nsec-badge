//
//  ble_battery.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-02.
//
//

#include "nsec_ble_internal.h"
#include <ble_bas.h>

static ble_bas_t _nsec_ble_bas;

static void _nsec_ble_battery_event_dispatch(ble_evt_t * p_evt);

void nsec_ble_battery_add(void) {
    ble_bas_init_t init;
    bzero(&init, sizeof(init));
    init.initial_batt_level = 50;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_report_read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&init.battery_level_char_attr_md.read_perm);

    APP_ERROR_CHECK(ble_bas_init(&_nsec_ble_bas, &init));
    nsec_ble_register_evt_handler(_nsec_ble_battery_event_dispatch);
}

static void _nsec_ble_battery_event_dispatch(ble_evt_t * p_evt) {
    ble_bas_on_ble_evt(&_nsec_ble_bas, p_evt);
}

void nsec_battery_set_level(uint8_t level) {
    ble_bas_battery_level_update(&_nsec_ble_bas, level);
}
