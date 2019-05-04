//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include "vendor_service.h"
#include "app/logs.h"
#include <app_error.h>
#include <ble.h>
#include "service_characteristic.h"
#include "uuid.h"


static void configure_characteristic_metadata(struct ServiceCharacteristic*, ble_gatts_char_md_t*, ble_gatts_attr_md_t*,
                                              ble_gatts_char_pf_t*, ble_gatts_attr_md_t*);
static void configure_characteristic_attribute(struct ServiceCharacteristic*, ble_gatts_attr_t*, ble_gatts_attr_md_t*);
static void set_default_metadata_for_attribute(struct ServiceCharacteristic*, ble_gatts_attr_md_t*);
static void configure_permission(struct ServiceCharacteristic*, ble_gatts_attr_md_t*);
static void set_characteristic_presentation_format(ble_gatts_char_pf_t*, uint8_t);
static void set_client_characteristic_configuration_declaration(struct ServiceCharacteristic*, ble_gatts_attr_md_t*);

void create_vendor_service(struct VendorService* service, uint16_t uuid){
    service->characteristic_count = 0;
    service->uuid = (ble_uuid_t){uuid, TYPE_NSEC_UUID};
}

void add_characteristic_to_vendor_service(struct VendorService* service, struct ServiceCharacteristic* characteristic){
    if(service->characteristic_count >= MAX_CHARACTERISTICS_PER_SERVICE)
        return;
    ble_gatts_char_handles_t characteristic_handles;
    service->characteristics[service->characteristic_count] = characteristic;
    ble_gatts_char_md_t metadata;
    ble_gatts_attr_md_t attribute_metadata;
    ble_gatts_attr_t attribute;
    ble_gatts_attr_md_t user_desc_metadata;
    ble_gatts_char_pf_t presentation_format;
    ble_gatts_attr_md_t cccd;
    configure_characteristic_metadata(characteristic, &metadata, &user_desc_metadata, &presentation_format, &cccd);
    configure_characteristic_attribute(characteristic, &attribute, &attribute_metadata);
    service->characteristic_count++;
    attribute.p_uuid = &(characteristic->uuid);
    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(service->handle, &metadata, &attribute, &characteristic_handles));
    characteristic->handle = characteristic_handles.value_handle;
}

struct ServiceCharacteristic* get_characteristic(struct VendorService* service, uint16_t characteristic_uuid){
    for(int i = 0; i < service->characteristic_count; i++){
        if(service->characteristics[i]->uuid.uuid == characteristic_uuid)
            return service->characteristics[i];
    }
    return NULL;
}

static void configure_characteristic_metadata(struct ServiceCharacteristic* characteristic,
                                              ble_gatts_char_md_t* char_metadata,
                                              ble_gatts_attr_md_t* user_desc_metadata, ble_gatts_char_pf_t* format,
                                              ble_gatts_attr_md_t* cccd){
    bzero(char_metadata, sizeof(ble_gatts_char_md_t));
    char_metadata->char_props.read = characteristic->read_mode != DENY_READ;
    char_metadata->char_props.write = characteristic->write_mode != DENY_WRITE;
    char_metadata->char_props.write_wo_resp = 0; // deactivate for now.
    char_metadata->char_props.notify = characteristic->allow_notify;
    if(characteristic->data_type != 0){
        set_characteristic_presentation_format(format, characteristic->data_type);
        char_metadata->p_char_pf = format;
    }
    else{
        char_metadata->p_char_pf = NULL;
    }
    char_metadata->p_user_desc_md = NULL;
    if(characteristic->allow_notify) {
        set_client_characteristic_configuration_declaration(characteristic, cccd);
        char_metadata->p_cccd_md = cccd;
    }
    else {
        char_metadata->p_cccd_md = NULL;
    }
    char_metadata->p_sccd_md = NULL;
    if(characteristic->user_descriptor != NULL){
        char_metadata->p_char_user_desc = (uint8_t*)characteristic->user_descriptor;
        char_metadata->char_user_desc_size = strlen(characteristic->user_descriptor);
        char_metadata->char_user_desc_max_size = char_metadata->char_user_desc_size;
        char_metadata->p_user_desc_md = user_desc_metadata;
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&user_desc_metadata->write_perm);
        user_desc_metadata->vlen = 0;
        user_desc_metadata->vloc = BLE_GATTS_VLOC_STACK;
        user_desc_metadata->rd_auth = 0;
        user_desc_metadata->wr_auth = 0;
        if(characteristic->read_permission == READ_OPEN)
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&user_desc_metadata->read_perm);
        else
            BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&user_desc_metadata->read_perm);
    }
}

static void configure_characteristic_attribute(struct ServiceCharacteristic* characteristic,
                                               ble_gatts_attr_t* attribute, ble_gatts_attr_md_t* attribute_metadata){
    attribute->init_len = characteristic->value_length;
    attribute->max_len = characteristic->value_length;
    attribute->init_offs = 0;
    set_default_metadata_for_attribute(characteristic, attribute_metadata);
    attribute->p_attr_md = attribute_metadata;
}

static void set_default_metadata_for_attribute(struct ServiceCharacteristic* characteristic,
                                               ble_gatts_attr_md_t* attribute_metadata){
    bzero(attribute_metadata, sizeof(*attribute_metadata));
    attribute_metadata->vloc = BLE_GATTS_VLOC_STACK;
    attribute_metadata->vlen = 0;
    configure_permission(characteristic, attribute_metadata);
}

static void configure_permission(struct ServiceCharacteristic* characteristic, ble_gatts_attr_md_t* attribute_metadata){
    if(characteristic->read_mode == DENY_READ)
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attribute_metadata->read_perm);
    else {
        if(characteristic->read_permission == READ_OPEN)
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attribute_metadata->read_perm);
        else
            BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attribute_metadata->read_perm);
    }

    attribute_metadata->rd_auth = characteristic->read_mode == REQUEST_READ;

    if(characteristic->write_mode == DENY_WRITE)
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attribute_metadata->write_perm);
    else {
        if(characteristic->write_permission == WRITE_OPEN)
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attribute_metadata->write_perm);
        else
            BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&attribute_metadata->write_perm);
    }
    attribute_metadata->wr_auth = characteristic->write_mode == AUTH_WRITE_REQUEST;
}

static void set_characteristic_presentation_format(ble_gatts_char_pf_t* format, uint8_t data_type){
    format->desc = 0;
    format->exponent = 0;
    format->name_space = 0;
    format->unit = 0;
    format->format = data_type;
}

static void set_client_characteristic_configuration_declaration(struct ServiceCharacteristic* characteristic,
                                                                ble_gatts_attr_md_t* cccd){
    memset(cccd, 0, sizeof(ble_gatts_attr_md_t));
    if(characteristic->read_permission == READ_OPEN)
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd->read_perm);
    else
        BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&cccd->read_perm);
    if(characteristic->write_permission == WRITE_OPEN)
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd->write_perm);
    else
        BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&cccd->write_perm);
    cccd->vloc = BLE_GATTS_VLOC_STACK;
}
