//
//  identity.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-04-24.
//
//

#include "identity.h"
#include <stdint.h>
#include <stdio.h>
#include <nrf51.h>
#include "ble/nsec_ble.h"
#include "ssd1306.h"

#include "images/default_avatar_bitmap.c"

#define AVATAR_SIZE \
    ((NSEC_IDENTITY_AVATAR_WIDTH * NSEC_IDENTITY_AVATAR_HEIGHT + 1) / 8)

typedef struct {
    char name[32];
    uint8_t xp_reverse;
    uint8_t xp_crypto;
    uint8_t xp_web;
    uint8_t unlocked;
    uint8_t avatar[AVATAR_SIZE];
} nsec_identity_t;

static nsec_identity_t identity;
static nsec_ble_service_handle identity_ble_handle;

const uint8_t identity_ble_uuid[16] = {
    0xFB, 0x36, 0xDB, 0x4C, 0x68, 0x42, 0x40, 0xBC,
    0xBA, 0x95, 0xB3, 0x93, 0x99, 0x96, 0xA9, 0x6A
};
enum {
    IDENTITY_CHAR_UUID_NAME = 0x6e6d,
    IDENTITY_CHAR_UUID_XP_REVERSE = 0x7870,
    IDENTITY_CHAR_UUID_XP_CRYPTO,
    IDENTITY_CHAR_UUID_XP_WEB,
    IDENTITY_CHAR_UUID_AVATAR = 0x6176,
    IDENTITY_CHAR_UUID_UNLOCK_KEY = 0x6b79,
    IDENTITY_CHAR_UUID_IS_UNLOCKED,
};

#define UPDATE_BLE_CHARACTERISTIC(uuid, field) \
nsec_ble_set_charateristic_value(identity_ble_handle, uuid, &field, sizeof(field))

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

static void nsec_identity_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, uint8_t * content, size_t content_length);

void nsec_identitiy_init(void) {
    memset(identity.name, 0, sizeof(identity.name));
    strncpy(identity.name, "<default name>", sizeof(identity.name));
    memcpy(identity.avatar, default_avatar_bitmap, sizeof(identity.avatar));
    identity.xp_reverse = 0;
    identity.xp_crypto = 0;
    identity.xp_web = 0;
    identity.unlocked = 0;

    nsec_ble_characteristic_t c[] = {
        {
            .char_uuid = IDENTITY_CHAR_UUID_NAME,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = sizeof(identity.name),
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_XP_REVERSE,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = 1,
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_XP_CRYPTO,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = 1,
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_XP_WEB,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = 1,
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_AVATAR,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = AVATAR_SIZE,
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_UNLOCK_KEY,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = 8,
            .on_write = nsec_identity_ble_callback,
        }, {
            .char_uuid = IDENTITY_CHAR_UUID_IS_UNLOCKED,
            .permissions = NSEC_BLE_CHARACT_PERM_READ,
            .max_size = 1,
            .on_write = nsec_identity_ble_callback,
        },
    };
    nsec_ble_service_t srv = {
        .characteristics_count = sizeof(c) / sizeof(c[0]),
        .characteristics = c,
    };
    memcpy(srv.uuid, identity_ble_uuid, sizeof(srv.uuid));
    nsec_ble_register_vendor_service(&srv, &identity_ble_handle);

    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_NAME, identity.name);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_XP_REVERSE, identity.xp_reverse);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_XP_CRYPTO, identity.xp_crypto);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_XP_WEB, identity.xp_web);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_AVATAR, identity.avatar);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_IS_UNLOCKED, identity.unlocked);
}

void nsec_identity_draw(void) {
    gfx_fillRect(8, 8, 128-16, 64-16, BLACK);
    gfx_drawBitmapBg(8, 16, identity.avatar, NSEC_IDENTITY_AVATAR_WIDTH, NSEC_IDENTITY_AVATAR_HEIGHT, WHITE, BLACK);
    gfx_setCursor(16+16+8, 16);
    gfx_puts(identity.name);
    // TODO: Add progress bar
    gfx_update();
}

void nsec_identity_get_unlock_key(char * data, size_t length) {
    snprintf(data, length, "%04X", ((NRF_FICR->DEVICEID[1] & 0xFFFF) ^ 0xC3C3));
}

static void nsec_identity_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, uint8_t * content, size_t content_length) {
    switch (char_uuid) {
        case IDENTITY_CHAR_UUID_NAME: {
            if(identity.unlocked) {
                memset(identity.name, 0, sizeof(identity.name));
                strncpy(identity.name, (char *) content,
                        MIN(content_length + 1, sizeof(identity.name)));
                UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_NAME, identity.name);
            }
        }
        case IDENTITY_CHAR_UUID_AVATAR: {
            if(identity.unlocked && content_length == AVATAR_SIZE) {
                memcpy(identity.avatar, content, AVATAR_SIZE);
                UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_AVATAR, identity.avatar);
            }
        }
        case IDENTITY_CHAR_UUID_UNLOCK_KEY: {
            char key[8];
            nsec_identity_get_unlock_key(key, sizeof(key));
            if(content_length == strlen(key) && memcmp(key, content, content_length) == 0) {
                identity.unlocked = 1;
            }
            else {
                identity.unlocked = 0;
            }
            UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_IS_UNLOCKED, identity.unlocked);
        }
    }
}
