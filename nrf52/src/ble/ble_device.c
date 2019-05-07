//
//  ble_device.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2015-11-22.
//
//

#include "nsec_ble_internal.h"
#include <ble_conn_params.h>
#include <ble_advdata.h>
#include <ble_dis.h>
#include <app_timer.h>
#include <peer_manager.h>
#include <app_scheduler.h>
#include <nrf_ble_gatt.h>
#include <nrf_error.h>

#include "ble_device.h"
#include "ble_peer_manager.h"
#include "boards.h"
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <ble/common/ble_gatt_db.h>
#include <iot/common/iot_common.h>
#include "drivers/led_effects.h"
#include "app/logs.h"
#include "gap_configuration.h"
#include "nsec_ble.h"
#include "vendor_service.h"
#include "service_characteristic.h"
#include "uuid.h"
#include "abstract_ble_observer.h"
#include "ble_scan.h"
#include "drivers/uart.h"
#include "app/pairing_menu.h"
#include "app/persistency.h"

#define APP_BLE_OBSERVER_PRIO 3
#define PEER_ADDRESS_SIZE 6
#define MAX_VENDOR_SERVICE_COUNT 8
#define LONG_WRITE_MAX_LENGTH 200
#define MAX_BLE_OBSERVERS 10
NRF_BLE_GATT_DEF(m_gatt);


typedef struct{
    const char* device_name;
    ble_gap_adv_params_t advertising_parameters;
    uint16_t vendor_service_count;
    struct VendorService* vendor_services[MAX_VENDOR_SERVICE_COUNT];
    struct Advertiser* advertiser;
    struct BleObserver* ble_observers[MAX_BLE_OBSERVERS];
    uint8_t ble_observers_count;
    uint16_t connection_handle;
} BleDevice;

typedef struct{
    uint16_t characteristic_handle;
    uint16_t write_offset;
    uint16_t write_length;
    uint8_t write_buffer[1];
} QueuedWrite;

static BleDevice* ble_device = NULL;

static uint8_t* buffer = NULL;

static bool nsec_ble_is_enabled = false;
static bool nsec_ble_connected = false;

static void _nsec_ble_softdevice_init();
static void gatt_init();
//static void add_device_information_service(char * manufacturer_name, char * model, char * serial_number,
//        char * hw_revision, char * fw_revision, char * sw_revision);
static void on_characteristic_write_command_event(const ble_gatts_evt_write_t * write_event);
static void on_execute_queued_write_commands();
static void on_characteristic_write_request_event(const ble_gatts_evt_write_t * write_event, uint16_t connection_handle);
static void on_characteristic_read_request_event(const ble_gatts_evt_read_t * read_event, uint16_t connection_handle);
static struct ServiceCharacteristic* get_characteristic_from_uuid(uint16_t uuid);
static void reply_to_client_request(uint8_t operation, uint16_t status_code, uint16_t connection_handle,
        const uint8_t* data_buffer, uint16_t data_length);
static void on_prepare_write_request(const ble_gatts_evt_write_t * write_event, uint16_t connection_handle);
static void on_execute_queued_write_requests(uint16_t connection_handle);
static uint16_t get_characteristic_handle_for_queued_writes();
static struct ServiceCharacteristic* get_characteristic_from_handle(uint16_t handle);
static uint16_t parse_queued_write_events(CharacteristicWriteEvent* event);


ret_code_t create_ble_device(char* device_name){
    if(ble_device == NULL){
        _nsec_ble_softdevice_init();
        init_peer_manager();
        ble_device = malloc(sizeof(BleDevice));
        ble_device->device_name = device_name;
        ble_device->vendor_service_count = 0;
        for(int i = 0; i < MAX_VENDOR_SERVICE_COUNT; i++){
            ble_device->vendor_services[i] = NULL;
        }
        register_nsec_vendor_specific_uuid();
        ble_device->ble_observers_count = 0;
        gatt_init();
        nsec_ble_is_enabled = get_stored_ble_is_enabled();
        return NRF_SUCCESS;
    }
    return -1;
}

void set_advertiser(struct Advertiser* advertiser){
    ble_device->advertiser = advertiser;
}

void ble_start_advertising(){
    if(ble_device->advertiser == NULL)
        return;
    ble_device->advertiser->start_advertisement();
}

void ble_stop_advertising(){
    ble_device->advertiser->stop_advertisement();
}

void add_observer(struct BleObserver* observer){
    if(ble_device->ble_observers_count < MAX_BLE_OBSERVERS){
        ble_device->ble_observers[ble_device->ble_observers_count] = observer;
        ble_device->ble_observers_count++;
    }
}

void ble_device_start_scan(){
    configure_scan(true, 0, 100, 50);
    start_scan();
}

void ble_device_stop_scan(){
    stop_scan();
}

static void ble_event_handler(ble_evt_t const * p_ble_evt, void * p_context){
    switch (p_ble_evt->header.evt_id){
        case BLE_GAP_EVT_CONNECTED:
            nsec_ble_connected = true;
            ble_device->connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            if(nsec_ble_is_enabled)
                ble_start_advertising();
            nsec_ble_connected = false;
            break;
        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            const uint16_t conn = p_ble_evt->evt.gatts_evt.conn_handle;
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(conn, NULL, 0, 0));
            }
            break;

        case BLE_GAP_EVT_ADV_REPORT: {
            if (ble_device->ble_observers_count == 0) {
                break;
            }
            const ble_gap_evt_adv_report_t *rp = &p_ble_evt->evt.gap_evt.params.adv_report;
            for (int c = 0; c < ble_device->ble_observers_count; c++) {
                ble_device->ble_observers[c]->on_advertising_report(rp);
            }
            break;
        }
        case BLE_GATTS_EVT_WRITE:
        {
            const ble_gatts_evt_write_t * event = &p_ble_evt->evt.gatts_evt.params.write;
            if(event->op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW){
                on_execute_queued_write_commands();
            }
            else{
                on_characteristic_write_command_event(event);
            }
            break;
        }
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            uint8_t type = p_ble_evt->evt.gatts_evt.params.authorize_request.type;
            if(type == BLE_GATTS_AUTHORIZE_TYPE_READ){
                const ble_gatts_evt_read_t * event = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
                on_characteristic_read_request_event(event, p_ble_evt->evt.gatts_evt.conn_handle);
            }
            else if(type == BLE_GATTS_AUTHORIZE_TYPE_WRITE){
                const ble_gatts_evt_write_t * event = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;
                uint16_t connection_handle = p_ble_evt->evt.gatts_evt.conn_handle;
                switch(event->op){
                    case BLE_GATTS_OP_WRITE_REQ:
                        on_characteristic_write_request_event(event, connection_handle);
                        break;
                    case BLE_GATTS_OP_PREP_WRITE_REQ:
                        on_prepare_write_request(event, connection_handle);
                        break;
                    case BLE_GATTS_OP_EXEC_WRITE_REQ_NOW:
                        on_execute_queued_write_requests(connection_handle);
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        case BLE_EVT_USER_MEM_REQUEST:
        {
            buffer = malloc(LONG_WRITE_MAX_LENGTH);
            ble_user_mem_block_t memory_block;
            memory_block.p_mem = buffer;
            memory_block.len = LONG_WRITE_MAX_LENGTH;
            volatile uint32_t error_code = sd_ble_user_mem_reply(p_ble_evt->evt.common_evt.conn_handle, &memory_block);
            APP_ERROR_CHECK(error_code);
        }
            break;
        case BLE_EVT_USER_MEM_RELEASE:
            free(p_ble_evt->evt.common_evt.params.user_mem_release.mem_block.p_mem);
            buffer = NULL;
            break;
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
        {
            char passkey[7];
            memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, sizeof(passkey) - 1);
            passkey[6] = '\0';
            nsec_ble_show_pairing_menu(passkey);
            break;
        }
    }
    ble_device->advertiser->on_ble_advertising_event(p_ble_evt);
}

uint32_t add_vendor_service(struct VendorService* service){
    if(ble_device->vendor_service_count >= MAX_VENDOR_SERVICE_COUNT)
        return 1;
    uint32_t error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service->uuid, &service->handle);
    APP_ERROR_CHECK(error_code);
    ble_device->vendor_services[ble_device->vendor_service_count] = service;
    ble_device->vendor_service_count++;
    return 0;
}

static void gatt_init(){
    APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, NULL));
}

static void on_characteristic_write_command_event(const ble_gatts_evt_write_t * write_event){
    struct ServiceCharacteristic* characteristic = get_characteristic_from_uuid(write_event->uuid.uuid);
    if(characteristic != NULL && characteristic->on_write_operation_done != NULL){
        CharacteristicWriteEvent event = {
            .write_offset = write_event->offset,
            .data_length = write_event->len,
            .data_buffer = write_event->data,
            .uuid = write_event->uuid.uuid,
        };
        characteristic->on_write_operation_done(&event);
    }
}

static void on_execute_queued_write_commands(){
    APP_ERROR_CHECK(buffer == NULL);
    CharacteristicWriteEvent event;
    uint16_t characteristic_handle = parse_queued_write_events(&event);
    if(characteristic_handle != BLE_GATT_HANDLE_INVALID){
        struct ServiceCharacteristic* characteristic = get_characteristic_from_handle(characteristic_handle);
        if(characteristic != NULL && characteristic->on_write_operation_done != NULL){
            characteristic->on_write_operation_done(&event);
        }
    }
}

static void on_characteristic_write_request_event(const ble_gatts_evt_write_t * write_event, uint16_t connection_handle){
    struct ServiceCharacteristic* characteristic = get_characteristic_from_uuid(write_event->uuid.uuid);
    if(characteristic != NULL && characteristic->on_write_request != NULL){
        CharacteristicWriteEvent event = {
            .write_offset = write_event->offset,
            .data_length = write_event->len,
            .data_buffer = write_event->data,
            .uuid = write_event->uuid.uuid,
        };
        uint16_t status_code = characteristic->on_write_request(&event);
        const uint8_t* data_buffer = status_code == BLE_GATT_STATUS_SUCCESS ? event.data_buffer: NULL;
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, status_code, connection_handle, data_buffer,
                event.data_length);
    }
    else{
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED,
                        connection_handle, NULL, 0);
    }
}

static void on_characteristic_read_request_event(const ble_gatts_evt_read_t * read_event, uint16_t connection_handle){
    struct ServiceCharacteristic* characteristic = get_characteristic_from_uuid(read_event->uuid.uuid);
    if(characteristic != NULL && characteristic->on_read_request != NULL){
        CharacteristicReadEvent event = {
            .read_offset = read_event->offset,
            .uuid = read_event->uuid.uuid,
        };
        uint16_t status_code = characteristic->on_read_request(&event);
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_READ, status_code, connection_handle, NULL, characteristic->value_length);
    }
    else{
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_READ, BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED, connection_handle, NULL, 0);
    }
}

static void on_prepare_write_request(const ble_gatts_evt_write_t * write_event, uint16_t connection_handle){
    if(get_characteristic_handle_for_queued_writes() == write_event->handle){
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, BLE_GATT_STATUS_SUCCESS, connection_handle, NULL, 0);
    }
    else{
        /* Queuing multiple writes to different characteristics should be allowed, but it would be difficult to handle properly and simply. */
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED, connection_handle, write_event->data, write_event->len);
    }
}

static void on_execute_queued_write_requests(uint16_t connection_handle){
    APP_ERROR_CHECK(buffer == NULL);
    uint16_t status_code = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    CharacteristicWriteEvent event;
    uint16_t characteristic_handle = parse_queued_write_events(&event);
    if(characteristic_handle != BLE_GATT_HANDLE_INVALID){
        struct ServiceCharacteristic* characteristic = get_characteristic_from_handle(characteristic_handle);
        if(characteristic != NULL && characteristic->on_write_request != NULL){
            status_code = characteristic->on_write_request(&event);
        }
    }
    if(status_code == BLE_GATT_STATUS_SUCCESS){
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, status_code, connection_handle, NULL, 0);
    }
    else{
        reply_to_client_request(BLE_GATTS_AUTHORIZE_TYPE_WRITE, BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED,
                connection_handle, NULL, 0);
    }
}

static uint16_t parse_queued_write_events(CharacteristicWriteEvent* event){
    uint8_t* current_read_index_in_buffer = buffer;
    QueuedWrite* write = (QueuedWrite*)current_read_index_in_buffer;
    event->data_length = 0;
    uint16_t characteristic_handle = write->characteristic_handle;

    while(write->characteristic_handle != BLE_GATT_HANDLE_INVALID){
        if(current_read_index_in_buffer - buffer > LONG_WRITE_MAX_LENGTH){
            event->data_buffer = NULL;
            event->data_length = 0;
            event->write_offset = 0;
            return BLE_GATT_HANDLE_INVALID;
        }
        if(characteristic_handle != write->characteristic_handle){
            // Writing to different characteristic in a queued requests should be supported but it is not for the moment, so reject the write.
            // It should not happen to often.
            event->data_buffer = NULL;
            event->data_length = 0;
            return BLE_GATT_HANDLE_INVALID;
        }

        current_read_index_in_buffer += sizeof(write->characteristic_handle) + sizeof(write->write_length)
                + sizeof(write->write_offset);
        characteristic_handle = write->characteristic_handle;
        uint16_t copy_size = write->write_length;
        uint8_t* copy_buffer = malloc(copy_size);
        // beyond this point, write doesn't point to a valid structure anymore, as the memcpy to defrag the data overwrite it.
        memcpy(copy_buffer, current_read_index_in_buffer, copy_size);
        memcpy(buffer + event->data_length, copy_buffer, copy_size);
        free(copy_buffer);
        event->data_length += copy_size;
        current_read_index_in_buffer += copy_size;
        write = (QueuedWrite*)current_read_index_in_buffer;
    }
    event->data_buffer = buffer;
    event->write_offset = 0;
    return characteristic_handle;
}

static void reply_to_client_request(uint8_t operation, uint16_t status_code, uint16_t connection_handle,
        const uint8_t* data_buffer, uint16_t data_length){
    ble_gatts_rw_authorize_reply_params_t reply;
    reply.type = operation;
    reply.params.read.gatt_status = status_code;
    reply.params.read.len = data_length;
    reply.params.read.offset = 0;
    reply.params.read.update = data_buffer == NULL ? false: true;
    reply.params.read.p_data = data_buffer;
    APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(connection_handle, &reply));
}

static struct ServiceCharacteristic* get_characteristic_from_uuid(uint16_t uuid){
    for(int i = 0; i < ble_device->vendor_service_count; i++){
        struct VendorService* service = ble_device->vendor_services[i];
        struct ServiceCharacteristic* characteristic = get_characteristic(service, uuid);
        if(characteristic != NULL)
            return characteristic;
    }
    return NULL;
}

static struct ServiceCharacteristic* get_characteristic_from_handle(uint16_t handle){
    for(int i = 0; i < ble_device->vendor_service_count; i++){
        struct VendorService* service = ble_device->vendor_services[i];
        for(int j = 0; j < service->characteristic_count; j++){
            struct ServiceCharacteristic* characteristic = service->characteristics[j];
            if(characteristic->handle == handle){
                return characteristic;
            }
        }
    }
    return NULL;
}

static uint16_t get_characteristic_handle_for_queued_writes(){
    if(buffer == NULL){
        return BLE_GATT_HANDLE_INVALID;
    }
    uint16_t handle = *((uint16_t*)buffer);
    return handle;
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

bool ble_device_toggle_ble()
{
    if (nsec_ble_is_enabled) {
        ble_device_stop_scan();
        ble_stop_advertising();
        if (nsec_ble_connected)
            sd_ble_gap_disconnect(ble_device->connection_handle,
                                  BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        nsec_ble_is_enabled = false;
    } else {
        ble_device_start_scan();
        ble_start_advertising();
        nsec_ble_is_enabled = true;
    }
    update_stored_ble_is_enabled(nsec_ble_is_enabled);
    return nsec_ble_is_enabled;
}

bool is_ble_enabled(void)
{
    return nsec_ble_is_enabled;
}

void ble_device_notify_characteristic(struct ServiceCharacteristic* characteristic, const uint8_t* value){
    if(nsec_ble_connected){
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));
        uint16_t length = characteristic->value_length;

        hvx_params.handle = characteristic->handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &length;
        hvx_params.p_data = value;
        uint32_t error_code = sd_ble_gatts_hvx(ble_device->connection_handle, &hvx_params);
        if(error_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING){
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(ble_device->connection_handle, NULL, 0, 0)); //init system attributes
        }
        else if(error_code != NRF_ERROR_RESOURCES && error_code != NRF_ERROR_INVALID_STATE) {
            APP_ERROR_CHECK(error_code);
        }
    }
}
