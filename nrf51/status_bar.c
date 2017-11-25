//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "status_bar.h"
#include "ssd1306.h"
#include <string.h>

#include "images/ble_logo_bitmap.c"
#include "images/battery_charging_bitmap.c"
#include "images/battery_bitmap.c"

static char status_bar_name[9];
static char badge_class[10];
static status_bluetooth_status ble_status;
static status_battery_state battery_state;

void nsec_status_bar_init() {
    status_bar_name[0] = '\0';
    badge_class[0] = '\0';
    nsec_status_bar_ui_redraw();
}

void nsec_status_set_name(char * name) {
    strncpy(status_bar_name, name, sizeof(status_bar_name) - 1);
    nsec_status_bar_ui_redraw();
}

void nsec_status_set_badge_class(char * class) {
    strncpy(badge_class, class, sizeof(badge_class) - 1);
    nsec_status_bar_ui_redraw();
}

void nsec_status_set_ble_status(status_bluetooth_status status) {
    if(ble_status != status) {
        ble_status = status;
        nsec_status_bar_ui_redraw();
    }
}

void nsec_status_set_battery_status(status_battery_state state) {
    if(battery_state != state) {
        battery_state = state;
        nsec_status_bar_ui_redraw();
    }
}

void nsec_status_bar_ui_redraw(void) {
    gfx_fillRect(0, 0, 128, 8, BLACK);
    gfx_setCursor(0, 0);
    gfx_setTextBackgroundColor(WHITE, BLACK);
    gfx_puts(status_bar_name);
    gfx_setCursor(50, 0);
    gfx_setTextBackgroundColor(BLACK, WHITE);
    gfx_puts(badge_class);
    gfx_setTextBackgroundColor(WHITE, BLACK);
    if(ble_status == STATUS_BLUETOOTH_ON) {
        gfx_drawBitmap(128 - 18, 0, ble_logo_bitmap, ble_logo_bitmap_width, ble_logo_bitmap_height, WHITE);
    }
    if(battery_state == STATUS_BATTERY_CHARGING) {
        gfx_drawBitmap(128 - 11, 0, battery_charging_bitmap, battery_charging_bitmap_width, battery_charging_bitmap_height, WHITE);
    }
    else {
        gfx_drawBitmap(128 - 11, 1, battery_bitmap, battery_bitmap_width, battery_bitmap_height, WHITE);
        uint8_t width;
        switch (battery_state) {
            case STATUS_BATTERY_25_PERCENT:
                width = 2;
                break;
            case STATUS_BATTERY_50_PERCENT:
                width = 4;
                break;
            case STATUS_BATTERY_75_PERCENT:
                width = 5;
                break;
            case STATUS_BATTERY_100_PERCENT:
                width = 7;
                break;
            default:
                width = 0;
                break;
        }
        gfx_fillRect(128 - 10, 2, width, 3, WHITE);
    }
    gfx_update();
}
