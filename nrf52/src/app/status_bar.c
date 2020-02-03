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

void nsec_status_bar_init(void)
{

    gfx_fill_rect(STATUS_BAR_POS, STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT,
                  STATUS_BAR_BG_COLOR);
    gfx_update();
}

void nsec_status_set_ble_status(status_bluetooth_status status)
{
    ble_status = status;

    if (ble_status == STATUS_BLUETOOTH_ON) {
        gfx_draw_16bit_bitmap(BLE_LOGO_POS, &ble_logo_bitmap, STATUS_BAR_BG_COLOR);
    }
    else{
        gfx_fill_rect(BLE_LOGO_POS, ble_logo_bitmap.width, ble_logo_bitmap.height, STATUS_BAR_BG_COLOR);
    }
}

void nsec_status_set_battery_status(status_battery_state state)
{
    battery_state = state;
    if (battery_state == STATUS_BATTERY_CHARGING) {
        gfx_draw_16bit_bitmap(BATTERY_POS, &battery_charging_bitmap,
                              STATUS_BAR_BG_COLOR);
    } else {
        gfx_draw_16bit_bitmap(BATTERY_POS, &battery_bitmap,
                              STATUS_BAR_BG_COLOR);
        switch (battery_state) {
        case STATUS_BATTERY_25_PERCENT:
            gfx_fill_rect(BATTERY_POS_X + 1, BATTERY_POS_Y + 1, 3, 6,
                          DISPLAY_RED);
            break;
        case STATUS_BATTERY_50_PERCENT:
            gfx_fill_rect(BATTERY_POS_X + 1, BATTERY_POS_Y + 1, 6, 6,
                          DISPLAY_YELLOW);
            break;
        case STATUS_BATTERY_75_PERCENT:
            gfx_fill_rect(BATTERY_POS_X + 1, BATTERY_POS_Y + 1, 9, 6,
                          DISPLAY_GREEN);
            break;
        case STATUS_BATTERY_100_PERCENT:
            gfx_fill_rect(BATTERY_POS_X + 1, BATTERY_POS_Y + 1, 13, 6,
                          DISPLAY_GREEN);
            break;
        default:
            gfx_fill_rect(BATTERY_POS_X + 1, BATTERY_POS_Y + 1, 0, 6,
                          DISPLAY_GREEN);
            break;
        }
    }
    gfx_update();
}

void nsec_status_bar_ui_redraw(void)
{
    gfx_fill_rect(gfx_get_screen_width() - STATUS_BAR_WIDTH, 0, STATUS_BAR_WIDTH,
                  STATUS_BAR_HEIGHT, STATUS_BAR_BG_COLOR);

    nsec_status_set_ble_status(ble_status);
    nsec_status_set_battery_status(battery_state);
}
