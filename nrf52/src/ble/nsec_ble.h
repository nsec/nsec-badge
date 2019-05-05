//
//  nsec_ble.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2015-11-22.
//
//

#ifndef nsec_ble_h
#define nsec_ble_h

#include <stdint.h>
#include <stdlib.h>

#define NSEC_BLE_LIMIT_MAX_VENDOR_SERVICE_COUNT (2)
#define NSEC_BLE_LIMIT_MAX_VENDOR_CHAR_COUNT (8)

struct nsec_ble_service_handle_s;
typedef struct nsec_ble_service_handle_s * nsec_ble_service_handle;

typedef void (*nsec_ble_characteristic_write_callback)(nsec_ble_service_handle service, uint16_t char_uuid, const uint8_t * content, size_t content_length);

typedef void (*nsec_ble_found_nsec_badge_callback)(uint16_t badge_id, const uint8_t addr[], int8_t rssi);

enum NSEC_BLE_CHARACT_PERM {
    NSEC_BLE_CHARACT_PERM_NONE  = 0,
    NSEC_BLE_CHARACT_PERM_READ  = (1 << 0),
    NSEC_BLE_CHARACT_PERM_WRITE = (1 << 1),
    NSEC_BLE_CHARACT_PERM_RW    = NSEC_BLE_CHARACT_PERM_READ | NSEC_BLE_CHARACT_PERM_WRITE,
};

typedef struct {
    uint16_t char_uuid;
    uint16_t max_size;
    uint8_t permissions; // see NSEC_BLE_CHARACT_PERM
    nsec_ble_characteristic_write_callback on_write;
} nsec_ble_characteristic_t;

typedef struct {
    uint8_t uuid[16];
    size_t characteristics_count;
    nsec_ble_characteristic_t * characteristics;
} nsec_ble_service_t;


int nsec_ble_init();

int nsec_ble_set_charateristic_value(nsec_ble_service_handle service, uint16_t characteristic_uuid, void * value, uint16_t value_size);

uint8_t nsec_ble_toggle(void);

// returns handle
int nsec_ble_register_vendor_service(nsec_ble_service_t * srv, nsec_ble_service_handle * handle);

void nsec_ble_hid_add_device(void);
void nsec_ble_battery_add(void);

void nsec_ble_set_scan_callback(nsec_ble_found_nsec_badge_callback callback);

#endif /* nsec_ble_h */
