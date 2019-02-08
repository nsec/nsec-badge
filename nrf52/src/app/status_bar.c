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

#include "images/ble_logo_bitmap.c"
#include "images/battery_charging_bitmap.c"
#include "images/battery_bitmap.c"

static char badge_class[10];
static status_bluetooth_status ble_status;
static status_battery_state battery_state;
extern uint16_t gfx_width;
extern uint16_t gfx_height;

void nsec_status_bar_init() {
    badge_class[0] = '\0';
    gfx_fill_rect(gfx_width - STATUS_BAR_WIDTH, 0, STATUS_BAR_WIDTH,
        STATUS_BAR_HEIGHT, STATUS_BAR_BG_COLOR);
    gfx_update();
}

// TODO Fit under battery
// TODO Do something to fit it in the midle
void nsec_status_set_badge_class(char * class) {
    strncpy(badge_class, class, sizeof(badge_class) - 1);
    gfx_set_cursor(135, 16);
    // TODO change text color ???
    gfx_set_text_background_color(DISPLAY_WHITE, STATUS_BAR_BG_COLOR);
    gfx_puts(badge_class);
    gfx_update();
}

// TODO use the correct PNG
void nsec_status_set_ble_status(status_bluetooth_status status) {
    ble_status = status;
    gfx_set_text_background_color(DISPLAY_WHITE, STATUS_BAR_BG_COLOR);
    if(ble_status == STATUS_BLUETOOTH_ON) {
        gfx_draw_bitmap(132, 6, ble_logo_bitmap, ble_logo_bitmap_width,
            ble_logo_bitmap_height, DISPLAY_BLUE);
    }
}

// TODO use the correct PNG
void nsec_status_set_battery_status(status_battery_state state)
{
    battery_state = state;
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    if(battery_state == STATUS_BATTERY_CHARGING) {
        gfx_draw_bitmap(140, 6, battery_charging_bitmap,
            battery_charging_bitmap_width, battery_charging_bitmap_height,
            DISPLAY_WHITE);
    } else {
        gfx_draw_bitmap(140, 6, battery_bitmap, battery_bitmap_width,
            battery_bitmap_height, DISPLAY_WHITE);
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
        gfx_fill_rect(142, 8, width, 3, DISPLAY_GREEN);
    }
    gfx_update();
}

void nsec_status_bar_ui_redraw(void)
{
    gfx_fill_rect(gfx_width - STATUS_BAR_WIDTH, 0, STATUS_BAR_WIDTH,
        STATUS_BAR_HEIGHT, STATUS_BAR_BG_COLOR);

    nsec_status_set_badge_class(badge_class);
    nsec_status_set_ble_status(ble_status);
    nsec_status_set_battery_status(battery_state);
}
