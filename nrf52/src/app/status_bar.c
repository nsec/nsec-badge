//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "status_bar.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include <string.h>

#include "images/battery_bitmap.h"
#include "images/battery_charging_bitmap.h"
#include "images/ble_logo_bitmap.h"

static status_bluetooth_status ble_status;
static status_battery_state battery_state;
extern uint16_t gfx_width;
extern uint16_t gfx_height;

static struct bitmap ble_logo, battery, battery_charging;

void nsec_status_bar_init() {
    ble_logo = (struct bitmap){
        .image = ble_logo_bitmap,
        .width = ble_logo_bitmap_width,
        .height = ble_logo_bitmap_height,
        .bg_color = STATUS_BAR_BG_COLOR
    };

    battery = (struct bitmap) {
        .image = battery_bitmap,
        .width = battery_bitmap_width,
        .height = battery_bitmap_height,
        .bg_color = STATUS_BAR_BG_COLOR
    };

    battery_charging = (struct bitmap) {
        .image = battery_charging_bitmap,
        .width = battery_charging_bitmap_width,
        .height = battery_charging_bitmap_height,
        .bg_color = STATUS_BAR_BG_COLOR
    };

    gfx_fill_rect(STATUS_BAR_POS, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT,
                    STATUS_BAR_BG_COLOR);
    gfx_update();
}

void nsec_status_set_ble_status(status_bluetooth_status status) {
    ble_status = status;
    gfx_draw_16bit_bitmap(BLE_LOGO_POS, &ble_logo);
    if(ble_status != STATUS_BLUETOOTH_ON) {
        //Draw a big red X on the logo
        uint16_t x = BLE_LOGO_POS_X;
        uint16_t y = BLE_LOGO_POS_Y;
            gfx_draw_bitmap(gfx_width - 18, 0, ble_logo_bitmap, ble_logo_bitmap_width, ble_logo_bitmap_height, DISPLAY_BLUE);
                            ble_logo_bitmap.width, ble_logo_bitmap.height,
                            DISPLAY_BLUE);
        gfx_draw_line(x, y+1, x + ble_logo.width, y + ble_logo.height+1, DISPLAY_RED);
        gfx_draw_line(x + ble_logo.width, y, x, y + ble_logo.height, DISPLAY_RED);
        gfx_draw_line(x + ble_logo.width, y+1, x, y + ble_logo.height+1, DISPLAY_RED);
    }
}

void nsec_status_set_battery_status(status_battery_state state)
{
    battery_state = state;
    if(battery_state == STATUS_BATTERY_CHARGING) {
            gfx_draw_bitmap(gfx_width - 11, 0, battery_charging_bitmap, battery_charging_bitmap_width, battery_charging_bitmap_height, DISPLAY_WHITE);
                            battery_charging_bitmap.width,
                            battery_charging_bitmap.height, DISPLAY_WHITE);
    } else {
            gfx_draw_bitmap(gfx_width - 11, 1, battery_bitmap, battery_bitmap_width, battery_bitmap_height, DISPLAY_WHITE);
                            battery_bitmap.width, battery_bitmap.height,
                            DISPLAY_WHITE);
        switch (battery_state) {
            case STATUS_BATTERY_25_PERCENT:
                gfx_fill_rect(BLE_LOGO_POS_X+1, BATTERY_POS_Y+1, 3, 6, DISPLAY_RED);
                break;
            case STATUS_BATTERY_50_PERCENT:
                gfx_fill_rect(BLE_LOGO_POS_X+1, BATTERY_POS_Y+1, 6, 6, DISPLAY_YELLOW);
                break;
            case STATUS_BATTERY_75_PERCENT:
                gfx_fill_rect(BLE_LOGO_POS_X+1, BATTERY_POS_Y+1, 9, 6, DISPLAY_GREEN);
                break;
            case STATUS_BATTERY_100_PERCENT:
                gfx_fill_rect(BLE_LOGO_POS_X+1, BATTERY_POS_Y+1, 13, 6, DISPLAY_GREEN);
                break;
            default:
                gfx_fill_rect(BLE_LOGO_POS_X+1, BATTERY_POS_Y+1, 0, 6, DISPLAY_GREEN);
                break;
        }
    }
    gfx_update();
}

void nsec_status_bar_ui_redraw(void)
{
    gfx_fill_rect(gfx_width - STATUS_BAR_WIDTH, 0, STATUS_BAR_WIDTH,
        STATUS_BAR_HEIGHT, STATUS_BAR_BG_COLOR);

    nsec_status_set_ble_status(ble_status);
    nsec_status_set_battery_status(battery_state);
        gfx_draw_bitmap(gfx_width - 18, 0, ble_logo_bitmap, ble_logo_bitmap_width, ble_logo_bitmap_height, DISPLAY_BLUE);
                        ble_logo_bitmap.width, ble_logo_bitmap.height,
                        DISPLAY_BLUE);
        gfx_draw_bitmap(gfx_width - 11, 0, battery_charging_bitmap, battery_charging_bitmap_width, battery_charging_bitmap_height, DISPLAY_WHITE);
                        battery_charging_bitmap.width,
                        battery_charging_bitmap.height, DISPLAY_WHITE);
        gfx_draw_bitmap(gfx_width - 11, 1, battery_bitmap, battery_bitmap_width, battery_bitmap_height, DISPLAY_WHITE);
                        battery_bitmap.width, battery_bitmap.height,
                        DISPLAY_WHITE);
}
