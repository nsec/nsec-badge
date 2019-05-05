//
//  ble_device_info.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-04-29.
//
//

#include "nsec_ble_internal.h"
#include <ble_dis.h>


#define MANUFACTURER "NeuroSoft"
#define MODEL "Beyond Reality Apex"
#define HARDWARE_REV "28C"
#define FIRMWARE_REV "2019.05.17"
#ifdef NSEC_FLAVOR_CTF
#define SOFTWARE_REV "FLAG-43f7087911c28422397dd50b8517d351"
#else
#define SOFTWARE_REV "2.3.4"
#endif

static void _nsec_ble_add_device_information_service_raw(ble_dis_init_t * raw_dis);

void nsec_ble_init_device_information_service() {
    ble_dis_init_t device_information;
    bzero(&device_information, sizeof(device_information));

    ble_srv_ascii_to_utf8(&device_information.manufact_name_str, MANUFACTURER);
    ble_srv_ascii_to_utf8(&device_information.model_num_str, MODEL);
    ble_srv_ascii_to_utf8(&device_information.hw_rev_str, HARDWARE_REV);
    ble_srv_ascii_to_utf8(&device_information.fw_rev_str, FIRMWARE_REV);
    ble_srv_ascii_to_utf8(&device_information.sw_rev_str, SOFTWARE_REV);

    _nsec_ble_add_device_information_service_raw(&device_information);
}

static void _nsec_ble_add_device_information_service_raw(ble_dis_init_t * raw_dis) {
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&raw_dis->dis_attr_md.read_perm);
    APP_ERROR_CHECK(ble_dis_init(raw_dis));
}
