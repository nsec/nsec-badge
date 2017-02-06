//
//  animal_care.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-07.
//
//

#include "animal_care.h"
#include "strings.h"
#include "ble/nsec_ble.h"
#include "ssd1306.h"
#include "controls.h"
#include "app_glue.h"

#include <string.h>
#include <stdbool.h>

#include <nrf_soc.h>
#include <app_timer.h>

#include "images/beer_icon_bitmap.c"
#include "images/socal_icon_bitmap.c"
#include "images/poo_icon_bitmap.c"
#include "images/animal_1_bitmap.c"
#include "images/nsec_logo_tiny_bitmap.c"
#include "images/cat_eye_bitmap.c"
#include "images/cat_eye_closed_bitmap.c"
#include "images/cat_eye_dead_bitmap.c"
#include "images/cat_party_hat_bitmap.c"
#include "images/poo_bitmap.c"
#include "images/poo_inside_bitmap.c"

typedef struct {
    char name[16];
    uint16_t sec_to_beer_death;
    uint16_t sec_to_social_death;
    uint16_t sec_to_next_caca;
    uint32_t sec_lived;
    uint8_t caca_count;
    struct {
        uint8_t x;
        uint8_t y;
    } caca_locations[5];
    uint8_t unlocked;
    uint8_t is_dead;
} nsec_animal_state_t;

nsec_animal_state_t animal_state;
APP_TIMER_DEF(animal_timer);
nsec_ble_service_handle animal_ble_handle;

static void animal_each_second(void * context);
static void animal_ui_update(void);
static void animal_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, uint8_t * content, size_t content_length);
static void animal_ui_draw_caca(uint8_t x, uint8_t y);
static void animal_ui_draw_name(void);
static void animal_ui_redraw_all(void);
static void animal_button_handler(button_t button);

const uint8_t animal_ble_uuid[16] = { 0x58, 0x8E, 0xBB, 0x09, 0x04, 0xE2, 0x49, 0x69, 0xB3, 0x36, 0x7C, 0xE7, 0xF6, 0xFE, 0xB3, 0x77 };
enum {
    ANIMAL_CHAR_UUID_NAME = 0x4141,
    ANIMAL_CHAR_UUID_SEC_TO_BEER_DEATH = 0x4545,
    ANIMAL_CHAR_UUID_SEC_TO_SOCIAL_DEATH,
    ANIMAL_CHAR_UUID_CACA_COUNT,
    ANIMAL_CHAR_UUID_UNLOCK_KEY,
    ANIMAL_CHAR_UUID_IS_UNLOCKED,
    ANIMAL_CHAR_UUID_IS_DEAD,
};

static bool _is_showing = false;

const char animal_unlock_password[] = "L33t h4x0r k3y";

#define UPDATE_BLE_CHARACTERISTIC(uuid, field) \
    nsec_ble_set_charateristic_value(animal_ble_handle, uuid, &field, sizeof(field))

#define DRAW_BITMAP(x, y, image) \
    gfx_drawBitmapBg(x, y, image ## _bitmap, image ## _bitmap_width, image ## _bitmap_height, WHITE, BLACK);

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

void animal_state_reset(void) {
    strncpy(animal_state.name, "Scriptkitty", sizeof(animal_state.name));
    animal_state.sec_to_beer_death   = 60 * 60 * 12;
    animal_state.sec_to_social_death = 60 * 60 * 12;
    animal_state.sec_to_next_caca    = 60 * 60 * 2;
    animal_state.sec_lived = 0;
    animal_state.unlocked = 0;
    animal_state.caca_count = 0;
    animal_state.is_dead = 0;
}

void animal_init(void) {
    animal_state_reset();
    app_timer_create(&animal_timer, APP_TIMER_MODE_REPEATED, animal_each_second);
    app_timer_start(animal_timer, APP_TIMER_TICKS(1000, 0), &animal_state);
    nsec_ble_characteristic_t c[] = {
        {
            .char_uuid = ANIMAL_CHAR_UUID_NAME,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_SEC_TO_BEER_DEATH,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_SEC_TO_SOCIAL_DEATH,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_CACA_COUNT,
            .permissions = NSEC_BLE_CHARACT_PERM_READ,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_UNLOCK_KEY,
            .permissions = NSEC_BLE_CHARACT_PERM_RW,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_IS_UNLOCKED,
            .permissions = NSEC_BLE_CHARACT_PERM_READ,
            .on_write = animal_ble_callback,
        }, {
            .char_uuid = ANIMAL_CHAR_UUID_IS_DEAD,
            .permissions = NSEC_BLE_CHARACT_PERM_READ,
            .on_write = animal_ble_callback,
        },
    };
    nsec_ble_service_t srv = {
        .characteristics_count = sizeof(c) / sizeof(c[0]),
        .characteristics = c,
    };
    memcpy(srv.uuid, animal_ble_uuid, sizeof(srv.uuid));
    nsec_ble_register_vendor_service(&srv, &animal_ble_handle);
    UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_NAME, animal_state.name);
    UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_CACA_COUNT, animal_state.caca_count);
    UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_IS_DEAD, animal_state.is_dead);
    UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_IS_UNLOCKED, animal_state.unlocked);
    nsec_controls_add_handler(animal_button_handler);
}

static void animal_each_second(void * context) {
    if(animal_state.is_dead) {
        return;
    }
    animal_state.sec_to_beer_death -= 1;
    UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_SEC_TO_BEER_DEATH, animal_state.sec_to_beer_death);
    //animal_state.sec_to_social_death -= 1;
    //UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_SEC_TO_SOCIAL_DEATH, animal_state.sec_to_social_death);
    animal_state.sec_to_next_caca -= 1;
    animal_state.sec_lived += 1;

    if(animal_state.sec_to_next_caca == 0) {
        uint16_t r;
        sd_rand_application_vector_get((uint8_t *)&r, sizeof(r));
        animal_state.caca_locations[animal_state.caca_count].x = 0 + r % 110;
        sd_rand_application_vector_get((uint8_t *)&r, sizeof(r));
        animal_state.caca_locations[animal_state.caca_count].y = 25 + r % 15;
        animal_ui_draw_caca(animal_state.caca_locations[animal_state.caca_count].x,
                            animal_state.caca_locations[animal_state.caca_count].y);
        animal_state.caca_count += 1;

        UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_CACA_COUNT, animal_state.caca_count);
        animal_state.sec_to_next_caca = 60 * 60 * 2;
    }

    if(animal_state.caca_count >= 5 ||
       animal_state.sec_to_beer_death == 0 ||
       animal_state.sec_to_social_death == 0) {
        animal_state.is_dead = 1;
    }

    animal_ui_update();
}

static void animal_ble_callback(nsec_ble_service_handle service, uint16_t char_uuid, uint8_t * content, size_t content_length) {
    switch (char_uuid) {
        case ANIMAL_CHAR_UUID_UNLOCK_KEY: {
            if (strncmp((char*) content, animal_unlock_password, MIN(content_length, strlen(animal_unlock_password))) == 0) {
                animal_state.unlocked = 1;
                UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_IS_UNLOCKED, animal_state.unlocked);
            }
            else {
                animal_state.unlocked = 0;
                UPDATE_BLE_CHARACTERISTIC(ANIMAL_CHAR_UUID_IS_UNLOCKED, animal_state.unlocked);
            }
        }
            break;
        case ANIMAL_CHAR_UUID_SEC_TO_BEER_DEATH: {
            if(animal_state.unlocked && content_length >= sizeof(animal_state.sec_to_beer_death)) {
                animal_state.sec_to_beer_death = *(uint16_t*)content;
            }
        }
            break;
        case ANIMAL_CHAR_UUID_SEC_TO_SOCIAL_DEATH: {
            if(animal_state.unlocked && content_length >= sizeof(animal_state.sec_to_social_death)) {
                animal_state.sec_to_social_death = *(uint16_t*)content;
            }
        }
            break;
        case ANIMAL_CHAR_UUID_NAME:
            strncpy(animal_state.name, (char*)content, sizeof(animal_state.name));
            animal_ui_draw_name();
            gfx_update();
            break;
    }
}

void animal_show(void) {
    _is_showing = true;
    animal_ui_redraw_all();
}

static void animal_ui_update(void) {
    if(!_is_showing) {
        return;
    }
    int16_t bar_width = (13 * animal_state.sec_to_beer_death) / (60 * 60 * 12);
    gfx_drawFastHLine(74, 13, 13, BLACK);
    gfx_drawFastHLine(74, 13, bar_width, WHITE);

    //bar_width = (13 * animal_state.sec_to_social_death) / (60 * 60 * 12);
    bar_width = (13 * animal_state.caca_count) / 5;
    gfx_drawFastHLine(103, 13, 13, BLACK);
    gfx_drawFastHLine(103, 13, bar_width, WHITE);

    //bar_width = (13 * animal_state.caca_count) / 5;
    //gfx_drawFastHLine(88, 21, 13, BLACK);
    //gfx_drawFastHLine(88, 21, bar_width, WHITE);

    if(!animal_state.is_dead) {
        if(animal_state.sec_lived % 4 == 0) {
            DRAW_BITMAP(22, 25, cat_eye_closed);
            DRAW_BITMAP(33, 25, cat_eye_closed);
        }
        else {
            DRAW_BITMAP(22, 25, cat_eye);
            DRAW_BITMAP(33, 25, cat_eye);
        }
    }
    else {
        DRAW_BITMAP(22, 25, cat_eye_dead);
        DRAW_BITMAP(33, 25, cat_eye_dead);
    }

    if(animal_state.caca_count > 0 && !animal_state.is_dead) {
        gfx_setCursor(55, 24);
        gfx_setTextBackgroundColor(WHITE, BLACK);
        gfx_puts("Clean poo");
        gfx_setCursor(55 + 6 * 9 + 2, 24);
        gfx_setTextBackgroundColor(BLACK, WHITE);
        gfx_puts("\x1a"); // right arrow
    }

    if(animal_state.sec_lived > 60 * 60 * 24) {
        DRAW_BITMAP(27, 9, cat_party_hat);
    }

    gfx_update();
}

static void animal_ui_draw_caca(uint8_t x, uint8_t y) {
    if(!_is_showing) {
        return;
    }
    gfx_drawBitmap(x, y, poo_bitmap, poo_bitmap_width, poo_bitmap_height, WHITE);
    gfx_drawBitmap(x, y + 10, poo_inside_bitmap, poo_inside_bitmap_width, poo_inside_bitmap_height, BLACK);
}

static void animal_ui_draw_empty_progress_bar(uint16_t x, uint16_t y) {
    gfx_drawRect(x, y, 15, 3, BLACK);
    gfx_drawFastHLine(x + 1, y    , 13, WHITE);
    gfx_drawFastHLine(x + 1, y + 2, 13, WHITE);
    gfx_drawFastVLine(x,      y + 1, 1, WHITE);
    gfx_drawFastVLine(x + 14, y + 1, 1, WHITE);
}

void animal_ui_redraw_all(void) {
    if(!_is_showing) {
        return;
    }
    gfx_fillRect(0, 8, 128, 56, BLACK);
    gfx_drawFastHLine(0, 42, 128, WHITE);
    animal_ui_draw_empty_progress_bar(73, 12);
    animal_ui_draw_empty_progress_bar(102, 12);
    //animal_ui_draw_empty_progress_bar(87, 20);

    DRAW_BITMAP(66, 10, beer_icon);
    //DRAW_BITMAP(95, 11, socal_icon);
    //DRAW_BITMAP(80, 17, poo_icon);
    DRAW_BITMAP(95, 11, poo_icon);
    DRAW_BITMAP(12, 14, animal_1);
    DRAW_BITMAP(111, 34, nsec_logo_tiny);

    animal_ui_draw_name();
    for(int i = 0; i < animal_state.caca_count; i++) {
        animal_ui_draw_caca(animal_state.caca_locations[i].x, animal_state.caca_locations[i].y);
    }
    animal_ui_update();
    gfx_update();
}

static void animal_ui_draw_name(void) {
    if(!_is_showing) {
        return;
    }
    gfx_fillRect(10, 56, 6*16, 8, BLACK);
    gfx_setCursor(10, 56);
    gfx_setTextBackgroundColor(WHITE, BLACK);
    gfx_puts(animal_state.name);
}

static void animal_button_handler(button_t button) {
    if(!_is_showing) {
        return;
    }
    switch (button) {
        case BUTTON_UP:
            break;
        case BUTTON_LEFT:
            break;
        case BUTTON_DOWN:
            break;
        case BUTTON_RIGHT:
        case BUTTON_ENTER:
            if(animal_state.caca_count > 0 && !animal_state.is_dead) {
                animal_state.caca_count--;
                animal_ui_redraw_all();
            }
            break;
        case BUTTON_BACK:
            _is_showing = false;
            show_main_menu();
            break;

        default:
            break;
    }
}

