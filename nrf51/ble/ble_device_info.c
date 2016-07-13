//
//  ble_device_info.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-04-29.
//
//

#include "nsec_ble_internal.h"
#include <ble_dis.h>

static void _nsec_ble_add_device_information_service_raw(ble_dis_init_t * raw_dis);
static void _nsec_ble_device_info_uuid_provider(size_t * uuid_count, ble_uuid_t * uuids);

void nsec_ble_add_device_information_service(char * manufacturer_name,
                                             char * model,
                                             char * serial_number,
                                             char * hw_revision,
                                             char * fw_revision,
                                             char * sw_revision) {
    ble_dis_init_t device_information;
    bzero(&device_information, sizeof(device_information));

    if(manufacturer_name) {
        ble_srv_ascii_to_utf8(&device_information.manufact_name_str,
                              manufacturer_name);
    }
    if(model) {
        ble_srv_ascii_to_utf8(&device_information.model_num_str,
                              model);
    }
    if(serial_number) {
        ble_srv_ascii_to_utf8(&device_information.serial_num_str,
                              serial_number);
    }
    if(hw_revision) {
        ble_srv_ascii_to_utf8(&device_information.hw_rev_str,
                              hw_revision);
    }
    if(fw_revision) {
        ble_srv_ascii_to_utf8(&device_information.fw_rev_str,
                              fw_revision);
    }
    if(sw_revision) {
        ble_srv_ascii_to_utf8(&device_information.sw_rev_str,
                              sw_revision);
    }

    _nsec_ble_add_device_information_service_raw(&device_information);
    nsec_ble_register_adv_uuid_provider(_nsec_ble_device_info_uuid_provider);
}

static void _nsec_ble_add_device_information_service_raw(ble_dis_init_t * raw_dis) {
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&raw_dis->dis_attr_md.read_perm);
    APP_ERROR_CHECK(ble_dis_init(raw_dis));
}

static void _nsec_ble_device_info_uuid_provider(size_t * uuid_count, ble_uuid_t * uuids) {
    if(*uuid_count < 1) {
        return;
    }
    uuids[0] = (ble_uuid_t) {
        .uuid = BLE_UUID_DEVICE_INFORMATION_SERVICE,
        .type = BLE_UUID_TYPE_BLE
    };
    *uuid_count = 1;
}
