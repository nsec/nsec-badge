//
//  ble_vendor_service.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-04-29.
//
//

#include "nsec_ble_internal.h"

struct nsec_ble_service_handle_s {
    uint8_t is_used;
    uint16_t sd_ble_handle;
    ble_uuid_t uuid;
};

typedef struct {
    nsec_ble_characteristic_t definition;
    nsec_ble_service_handle service;
    uint8_t index;
    ble_gatts_char_handles_t sd_ble_handle;
} nsec_ble_characteristic_list_item_t;

static struct nsec_ble_service_handle_s _nsec_ble_vendor_services[NSEC_BLE_LIMIT_MAX_VENDOR_SERVICE_COUNT];
static nsec_ble_characteristic_list_item_t _nsec_ble_vendor_services_characteristics[NSEC_BLE_LIMIT_MAX_VENDOR_CHAR_COUNT];

static void _nsec_ble_add_caracteristic(nsec_ble_service_handle service_handle, nsec_ble_characteristic_t * charac, ble_gatts_char_handles_t * charac_handle);
static void _nsec_ble_vendor_uuid_provider(size_t * uuid_count, ble_uuid_t * uuids);
static void _nsec_ble_vendor_evt_handler(ble_evt_t * p_ble_evt);

int nsec_ble_register_vendor_service(nsec_ble_service_t * srv, nsec_ble_service_handle * handle) {
    ble_uuid128_t     base_uuid;

    bzero(_nsec_ble_vendor_services, sizeof(_nsec_ble_vendor_services));

    memcpy(&base_uuid.uuid128, srv->uuid, sizeof(base_uuid.uuid128));

    nsec_ble_service_handle new_service_handle = NULL;
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_VENDOR_SERVICE_COUNT; i++) {
        if(!_nsec_ble_vendor_services[i].is_used) {
            new_service_handle = &_nsec_ble_vendor_services[i];
            new_service_handle->is_used = 1;
            break;
        }
    }

    if(new_service_handle == NULL) {
        // NSEC_BLE_ERROR_TOO_MANY_VENDOR_SERVICES
        return -1;
    }

    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &new_service_handle->uuid.type));
    new_service_handle->uuid.uuid = srv->uuid[12] << 8 | srv->uuid[13];

    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &new_service_handle->uuid, &new_service_handle->sd_ble_handle));

    *handle = new_service_handle;

    for(int i = 0; i < srv->characteristics_count; i++) {
        nsec_ble_characteristic_list_item_t * charac_handle = NULL;
        for(int j = 0; j < NSEC_BLE_LIMIT_MAX_VENDOR_CHAR_COUNT; j++) {
            if(_nsec_ble_vendor_services_characteristics[j].service == NULL) {
                charac_handle = &_nsec_ble_vendor_services_characteristics[j];
                break;
            }
        }
        if(charac_handle == NULL) {
            // NSEC_BLE_ERROR_TOO_MANY_VENDOR_SERVICES_CHARACT
            return -1;
        }
        charac_handle->service = new_service_handle;
        charac_handle->index = i;
        charac_handle->definition = srv->characteristics[i];
        _nsec_ble_add_caracteristic(new_service_handle, &charac_handle->definition, &charac_handle->sd_ble_handle);
    }

    nsec_ble_register_adv_uuid_provider(_nsec_ble_vendor_uuid_provider);
    nsec_ble_register_evt_handler(_nsec_ble_vendor_evt_handler);
    return 0;
}

static void _nsec_ble_vendor_uuid_provider(size_t * uuid_count, ble_uuid_t * uuids) {
    size_t count = 0;
    for(int i = 0; i < NSEC_BLE_LIMIT_MAX_VENDOR_SERVICE_COUNT && i < *uuid_count; i++) {
        if(_nsec_ble_vendor_services[i].is_used) {
            uuids[count++] = _nsec_ble_vendor_services[i].uuid;
        }
    }
    *uuid_count = count;
}

static void _nsec_ble_vendor_evt_handler(ble_evt_t * p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE: {
            nsec_ble_characteristic_list_item_t * charac_item = NULL;
            for(int i = 0; i < NSEC_BLE_LIMIT_MAX_VENDOR_CHAR_COUNT; i++) {
                if(_nsec_ble_vendor_services_characteristics[i].definition.char_uuid == p_ble_evt->evt.gatts_evt.params.write.context.char_uuid.uuid &&
                   _nsec_ble_vendor_services_characteristics[i].service->sd_ble_handle == p_ble_evt->evt.gatts_evt.params.write.context.srvc_handle) {
                    charac_item = &_nsec_ble_vendor_services_characteristics[i];
                    break;
                }
            }
            if(charac_item != NULL) {
                charac_item->definition.on_write(charac_item->service,
                                                 charac_item->definition.char_uuid,
                                                 p_ble_evt->evt.gatts_evt.params.write.data,
                                                 p_ble_evt->evt.gatts_evt.params.write.len);
            }
            else {
                // Bad send request
            }
        }
            break;
    }
}

static void _nsec_ble_add_caracteristic(nsec_ble_service_handle service_handle, nsec_ble_characteristic_t * charac, ble_gatts_char_handles_t * charac_handle) {
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    bzero(&cccd_md, sizeof(cccd_md));
    bzero(&char_md, sizeof(char_md));
    bzero(&attr_md, sizeof(attr_md));
    bzero(&attr_char_value, sizeof(attr_char_value));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    if(charac->permissions & NSEC_BLE_CHARACT_PERM_READ) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    }
    else {
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    }

    if(charac->permissions & NSEC_BLE_CHARACT_PERM_WRITE) {
        char_md.char_props.write = 1;
        char_md.char_props.write_wo_resp = 1;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    }
    else {
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    }

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    ble_uuid_t char_uuid = {
        .uuid = charac->char_uuid,
        .type = service_handle->uuid.type
    };

    attr_char_value.p_uuid       = &char_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = (64 * sizeof(uint8_t)); // FIXME: hardcoded
    attr_char_value.p_value      = NULL;

    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(service_handle->sd_ble_handle, &char_md,
                                                    &attr_char_value,
                                                    charac_handle));
}

int nsec_ble_set_charateristic_value(nsec_ble_service_handle service, uint16_t characteristic_uuid, void * value, uint16_t value_size) {
    nsec_ble_characteristic_list_item_t * charac_handle = NULL;
    for(int j = 0; j < NSEC_BLE_LIMIT_MAX_VENDOR_CHAR_COUNT; j++) {
        if(_nsec_ble_vendor_services_characteristics[j].service == service &&
           _nsec_ble_vendor_services_characteristics[j].definition.char_uuid == characteristic_uuid) {
            charac_handle = &_nsec_ble_vendor_services_characteristics[j];
            break;
        }
    }
    if(charac_handle == NULL) {
        // NSEC_BLE_ERROR_CHARAC_NOT_FOUND
        return -1;
    }

    // TODO: Check if everything was written
    APP_ERROR_CHECK(sd_ble_gatts_value_set(charac_handle->sd_ble_handle.value_handle, 0, &value_size, value));
    return 0;
}

