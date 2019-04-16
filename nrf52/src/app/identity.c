//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "identity.h"
#include <stdint.h>
#include <stdio.h>
#include <nrf52.h>
#include <app_timer.h>
#include "ble/nsec_ble.h"
#include "nsec_nearby_badges.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "3d.h"
#include "persistency.h"

#include "ble/ble_device.h"
#include "ble/gap_configuration.h"
#include "ble/service_characteristic.h"
#include "ble/uuid.h"
#include "ble/vendor_service.h"
#include "images/default_avatar_bitmap.h"
#include "images/nsec_logo_tiny_bitmap.h"
#include "images/star_bitmap.h"

extern uint16_t gfx_width;
extern uint16_t gfx_height;

#define AVATAR_SIZE \
    ((NSEC_IDENTITY_AVATAR_WIDTH * NSEC_IDENTITY_AVATAR_HEIGHT + 1) / 8)

#define M_PI            3.14159265358979323846
#define UNLOCK_PASSWORD_SIZE 5

static uint16_t on_name_write(CharacteristicWriteEvent* event);
//static uint16_t on_avatar_write(CharacteristicWriteEvent* event);
static uint16_t on_unlock_password_write(CharacteristicWriteEvent* event);

static void configure_service();

#define NAME_MAX_LEN 16

typedef struct {
    char name[NAME_MAX_LEN + 1];
    uint8_t unlocked;
    uint8_t avatar[AVATAR_SIZE];
} BadgeIdentity;

static BadgeIdentity identity;
static struct VendorService identity_ble_service;
static ServiceCharacteristic name_characteristic;
static ServiceCharacteristic unlock_password_characteristic;
static ServiceCharacteristic unlocked_characteristic;
//static ServiceCharacteristic avatar_characteristic;

static uint16_t service_uuid = 0x0100;
static uint16_t name_char_uuid = 0x0101;
static uint16_t unlocked_password_char_uuid = 0x0102;
static uint16_t unlocked_char_uuid = 0x0103;


#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif


APP_TIMER_DEF(nsec_render_timer);

#if !(defined(NSEC_ROTATING_MESH))
#define NSEC_ROTATING_MESH nsec_cube
#endif

/*static void nsec_render_3d_mesh(void * context) {
    if(is_at_main_menu) {
        static float current_angle = 0.0f;
        gfx_fill_rect(0, 10, 40, 42, DISPLAY_BLACK);
        nsec_draw_rotated_mesh(NSEC_ROTATING_MESH, (int[2]) {20, 32}, 11,
                               (float[3]) {current_angle,
                                           current_angle + 1.0f,
                                           current_angle + 2.0f});
        current_angle += 0.03f;
        if(current_angle >= 2 * M_PI) {
            current_angle -= 2 * M_PI;
        }
        gfx_update();
    }
}*/

void init_identity_service() {
    memset(identity.name, 0, sizeof(identity.name));

    //app_timer_create(&nsec_render_timer, APP_TIMER_MODE_REPEATED, nsec_render_3d_mesh);
    //app_timer_start(nsec_render_timer, APP_TIMER_TICKS(40), NULL);

    load_stored_identity(identity.name);
    //memcpy(identity.avatar, default_avatar_bitmap, sizeof(identity.avatar));
    identity.unlocked = 0;
    configure_service();
}

/*static void nsec_draw_empty_progress_bar(uint16_t x, uint16_t y, uint16_t w) {
    gfx_drawRect(x, y, w, 3, DISPLAY_BLACK);
    gfx_drawFastHLine(x + 1, y    , w - 2, DISPLAY_WHITE);
    gfx_drawFastHLine(x + 1, y + 2, w - 2, DISPLAY_WHITE);
    gfx_drawFastVLine(x,         y + 1, 1, DISPLAY_WHITE);
    gfx_drawFastVLine(x + w - 1, y + 1, 1, DISPLAY_WHITE);
}*/

void nsec_identity_draw(void) {
    gfx_fill_rect(48, 12, gfx_width-48, 20, DISPLAY_BLACK);
    //gfx_draw_bitmapBg(8, 16, identity.avatar, NSEC_IDENTITY_AVATAR_WIDTH, NSEC_IDENTITY_AVATAR_HEIGHT, DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_set_cursor(48, 30);
    char name_with_spaces[NAME_MAX_LEN + 1];
    snprintf(name_with_spaces, NAME_MAX_LEN + 1, "%-14s", identity.name);
    gfx_puts(name_with_spaces);
    gfx_update();
    //gfx_draw_bitmap(59, 32, star_bitmap, star_bitmap_width, star_bitmap_height, DISPLAY_WHITE);
    //nsec_draw_empty_progress_bar(70, 38, 30);
    //gfx_draw_bitmapBg(111, 47, nsec_logo_tiny_bitmap, nsec_logo_tiny_bitmap_width, nsec_logo_tiny_bitmap_height, DISPLAY_WHITE, DISPLAY_BLACK);
    //nsec_identity_update_nearby();
}

void nsec_identity_update_nearby(void) {
    int16_t bar_width = (28 * nsec_nearby_badges_current_count()) / (NSEC_MAX_NEARBY_BADGES_COUNT);
    gfx_draw_fast_hline(71, 39, 28, DISPLAY_BLACK);
    gfx_draw_fast_hline(71, 39, bar_width, DISPLAY_WHITE);
}

void nsec_identity_get_unlock_key(char * data, size_t length) {
    snprintf(data, length, "%04lX", ((NRF_FICR->DEVICEID[1] % 0xFFFF) ^ 0xC3C3));
}

static uint16_t on_name_write(CharacteristicWriteEvent* event){
    if(identity.unlocked) {
        memset(identity.name, 0, NAME_MAX_LEN);
        strncpy(identity.name, (char *) event->data_buffer, MIN(event->data_length, NAME_MAX_LEN));
        update_identity(identity.name);
        return BLE_GATT_STATUS_SUCCESS;
    }
    else{
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }
}

/*static uint16_t on_avatar_write(CharacteristicWriteEvent* event){
	if(identity.unlocked && event->data_length == AVATAR_SIZE) {
		memcpy(identity.avatar, event->data_buffer, AVATAR_SIZE);
		if(is_at_main_menu) {
				nsec_identity_draw();
				gfx_update();
		}
		return BLE_GATT_STATUS_SUCCESS;
	}
	return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
}*/

static uint16_t on_unlock_password_write(CharacteristicWriteEvent* event){
    char unlock_password[UNLOCK_PASSWORD_SIZE];
    nsec_identity_get_unlock_key(unlock_password, UNLOCK_PASSWORD_SIZE);
    if(identity.unlocked){
        return BLE_GATT_STATUS_SUCCESS;
    }
    else if(event->data_length == (UNLOCK_PASSWORD_SIZE - 1) && memcmp(unlock_password, (const char*)event->data_buffer, UNLOCK_PASSWORD_SIZE) == 0){
        identity.unlocked = 1;
        set_characteristic_value(&unlocked_characteristic, &identity.unlocked);
        return BLE_GATT_STATUS_SUCCESS;
    }
    return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
}

static void configure_service(){
    create_vendor_service(&identity_ble_service, service_uuid);
    add_vendor_service(&identity_ble_service);

    create_characteristic(&name_characteristic, NAME_MAX_LEN, AUTO_READ, REQUEST_WRITE, name_char_uuid);
    add_characteristic_to_vendor_service(&identity_ble_service, &name_characteristic);
    add_write_request_handler(&name_characteristic, on_name_write);
    set_characteristic_value(&name_characteristic, (uint8_t*)identity.name);

    /*
    add_characteristic_to_vendor_service(&identity_ble_service, &avatar_characteristic, sizeof(identity.avatar), AUTO_READ, REQUEST_WRITE);
    add_write_request_handler(&avatar_characteristic, on_avatar_write);
    set_characteristic_value(&avatar_characteristic, identity.avatar);
    */

    create_characteristic(&unlock_password_characteristic, UNLOCK_PASSWORD_SIZE, DENY_READ, REQUEST_WRITE, unlocked_password_char_uuid);
    add_characteristic_to_vendor_service(&identity_ble_service, &unlock_password_characteristic);
    add_write_request_handler(&unlock_password_characteristic, on_unlock_password_write);

    create_characteristic(&unlocked_characteristic, sizeof(identity.unlocked), AUTO_READ, DENY_WRITE, unlocked_char_uuid);
    add_characteristic_to_vendor_service(&identity_ble_service, &unlocked_characteristic);
    set_characteristic_value(&unlocked_characteristic, &identity.unlocked);
}
