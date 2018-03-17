//
//  nsec_ble_internal.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-04-29.
//
//

#ifndef nsec_ble_internal_h
#define nsec_ble_internal_h

#include "nsec_ble.h"
#include <string.h>
#include <ble.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>

#define NSEC_BLE_LIMIT_MAX_EVENT_HANDLER (4)
#define NSEC_BLE_LIMIT_MAX_UUID_PROVIDER (4)

typedef void (*nsec_ble_adv_uuid_provider)(size_t * uuid_count, ble_uuid_t * uuids);

void nsec_ble_register_evt_handler(nrf_sdh_ble_evt_handler_t handler);
void nsec_ble_register_adv_uuid_provider(nsec_ble_adv_uuid_provider provider);


#endif /* nsec_ble_internal_h */
