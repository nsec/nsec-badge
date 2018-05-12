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
#include "ssd1306.h"
#include "nsec_nearby_badges.h"
#include "app_glue.h"
#include "3d.h"

#include "images/default_avatar_bitmap.c"
#include "images/nsec_logo_tiny_bitmap.c"
#include "images/star_bitmap.c"

#define AVATAR_SIZE \
    ((NSEC_IDENTITY_AVATAR_WIDTH * NSEC_IDENTITY_AVATAR_HEIGHT + 1) / 8)

#define M_PI            3.14159265358979323846

typedef struct {
#ifdef NSEC_SHOW_FLAG_ON_SUCCESSFUL_NAME_CHANGE
    // Use 32 bytes to accomodate the flag length.
    char name[32];
#else
    // We could probably just always use 32 bytes but the other firmware on the
    // badge used 16.
    char name[16];
#endif
    uint8_t unlocked;
    uint8_t avatar[AVATAR_SIZE];
} nsec_identity_t;

static nsec_identity_t identity;
static nsec_ble_service_handle identity_ble_handle;

const uint8_t identity_ble_uuid[16] = {
    // 6AA99996-93B3-95BA-BC40-42684CDB36FB
    0xFB, 0x36, 0xDB, 0x4C, 0x68, 0x42, 0x40, 0xBC,
    0xBA, 0x95, 0xB3, 0x93, 0x99, 0x96, 0xA9, 0x6A
};
enum {
    IDENTITY_CHAR_UUID_NAME = 0x6e6d,
    IDENTITY_CHAR_UUID_AVATAR = 0x6176,
    IDENTITY_CHAR_UUID_UNLOCK_KEY = 0x6b79,
    IDENTITY_CHAR_UUID_IS_UNLOCKED,
};

#ifdef NSEC_IDENTITY_BLE_SERVICE_DISABLE
#define UPDATE_BLE_CHARACTERISTIC(uuid, field)
#else
#define UPDATE_BLE_CHARACTERISTIC(uuid, field) \
nsec_ble_set_charateristic_value(identity_ble_handle, uuid, &field, sizeof(field))
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifdef NSEC_SHOW_FLAG_ON_SUCCESSFUL_NAME_CHANGE
static char flag[] = "flag{ble_all_the_things}";
#endif

APP_TIMER_DEF(nsec_render_timer);

static void nsec_identity_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, const uint8_t * content, size_t content_length);

static void nsec_render_3d_mesh(void * context) {
    if(is_at_main_menu) {
        static float current_angle = 0.0f;
        gfx_fillRect(0, 10, 40, 42, SSD1306_BLACK);
        nsec_draw_rotated_mesh(nsec_cube, (int[2]) {20, 32}, 11,
                               (float[3]) {current_angle,
                                           current_angle + 1.0f,
                                           current_angle + 2.0f});
        current_angle += 0.03f;
        if(current_angle >= 2 * M_PI) {
            current_angle -= 2 * M_PI;
        }
        gfx_update();
    }
}

void nsec_identity_init(void) {
    memset(identity.name, 0, sizeof(identity.name));

    app_timer_create(&nsec_render_timer, APP_TIMER_MODE_REPEATED, nsec_render_3d_mesh);
    app_timer_start(nsec_render_timer, APP_TIMER_TICKS(40), NULL);

#if defined(NSEC_HARDCODED_BADGE_IDENTITY_NAME)
#define NSEC_STRINGIFY_(...) #__VA_ARGS__
#define NSEC_STRINGIFY(...) NSEC_STRINGIFY_(__VA_ARGS__)
    snprintf(identity.name, sizeof(identity.name), NSEC_STRINGIFY(NSEC_HARDCODED_BADGE_IDENTITY_NAME));
#else
    snprintf(identity.name, sizeof(identity.name), "Comrade #%05ld", (NRF_FICR->DEVICEID[0] & 0xFFFF));
#endif
    memcpy(identity.avatar, default_avatar_bitmap, sizeof(identity.avatar));
    identity.unlocked = 0;

    nsec_ble_characteristic_t c[] = {
        {
            .char_uuid = IDENTITY_CHAR_UUID_NAME,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .max_size = sizeof(identity.name),
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
#ifndef NSEC_IDENTITY_BLE_SERVICE_DISABLE
    nsec_ble_register_vendor_service(&srv, &identity_ble_handle);
#endif

    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_NAME, identity.name);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_AVATAR, identity.avatar);
    UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_IS_UNLOCKED, identity.unlocked);
}

static void nsec_draw_empty_progress_bar(uint16_t x, uint16_t y, uint16_t w) {
    gfx_drawRect(x, y, w, 3, SSD1306_BLACK);
    gfx_drawFastHLine(x + 1, y    , w - 2, SSD1306_WHITE);
    gfx_drawFastHLine(x + 1, y + 2, w - 2, SSD1306_WHITE);
    gfx_drawFastVLine(x,         y + 1, 1, SSD1306_WHITE);
    gfx_drawFastVLine(x + w - 1, y + 1, 1, SSD1306_WHITE);
}

void nsec_identity_draw(void) {
    gfx_fillRect(8, 8, 128-16, 64-16, SSD1306_BLACK);
    gfx_drawBitmapBg(8, 16, identity.avatar, NSEC_IDENTITY_AVATAR_WIDTH, NSEC_IDENTITY_AVATAR_HEIGHT, SSD1306_WHITE, SSD1306_BLACK);
    gfx_setCursor(16+16+8, 20);
    char name_with_spaces[sizeof(identity.name)];
    snprintf(name_with_spaces, sizeof(name_with_spaces), "%-14s", identity.name);
    gfx_puts(name_with_spaces);
    gfx_drawBitmap(59, 32, star_bitmap, star_bitmap_width, star_bitmap_height, SSD1306_WHITE);
    nsec_draw_empty_progress_bar(70, 38, 30);
    gfx_drawBitmapBg(111, 47, nsec_logo_tiny_bitmap, nsec_logo_tiny_bitmap_width, nsec_logo_tiny_bitmap_height, SSD1306_WHITE, SSD1306_BLACK);
    nsec_identity_update_nearby();
}

void nsec_identity_update_nearby(void) {
    int16_t bar_width = (28 * nsec_nearby_badges_current_count()) / (NSEC_MAX_NEARBY_BADGES_COUNT);
    gfx_drawFastHLine(71, 39, 28, SSD1306_BLACK);
    gfx_drawFastHLine(71, 39, bar_width, SSD1306_WHITE);
}

void nsec_identity_get_unlock_key(char * data, size_t length) {
    snprintf(data, length, "%04lX", ((NRF_FICR->DEVICEID[1] % 0xFFFF) ^ 0xC3C3));
}

static void nsec_identity_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, const uint8_t * content, size_t content_length) {
    switch (char_uuid) {
        case IDENTITY_CHAR_UUID_NAME: {
            if(identity.unlocked) {
                memset(identity.name, 0, sizeof(identity.name));
#ifdef NSEC_SHOW_FLAG_ON_SUCCESSFUL_NAME_CHANGE
                strncpy(identity.name, (char *) flag,
                        MIN(sizeof(flag), sizeof(identity.name)));
#else
                strncpy(identity.name, (char *) content,
                        MIN(content_length, sizeof(identity.name)));
#endif
                UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_NAME, identity.name);
            }
            if(is_at_main_menu) {
                nsec_identity_draw();
                gfx_update();
            }
        }
        case IDENTITY_CHAR_UUID_AVATAR: {
            if(identity.unlocked && content_length == AVATAR_SIZE) {
                memcpy(identity.avatar, content, AVATAR_SIZE);
                UPDATE_BLE_CHARACTERISTIC(IDENTITY_CHAR_UUID_AVATAR, identity.avatar);
            }
            if(is_at_main_menu) {
                nsec_identity_draw();
                gfx_update();
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
