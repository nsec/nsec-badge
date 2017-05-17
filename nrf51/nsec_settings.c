//
//  nsec_settings.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-17.
//
//

#include "nsec_settings.h"
#include "menu.h"
#include "ssd1306.h"
#include <nrf.h>
#include "ble/nsec_ble.h"
#include "status_bar.h"
#include "app_glue.h"
#include "controls.h"
#include "identity.h"
#include <stdio.h>

static void toggle_bluetooth(uint8_t item);
static void show_credit(uint8_t item);
static void turn_off_screen(uint8_t item);
static void flashlight(uint8_t item);
static void reset_pet(uint8_t item);
static void setting_handle_buttons(button_t button);

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_CREDIT,
    SETTING_STATE_SCREEN_OFF,
    SETTING_STATE_FLASHLIGHT,
};

static enum setting_state _state = SETTING_STATE_CLOSED;

static void setting_handle_buttons(button_t button);

static char sync_key_string[] = "Sync key: XXXX";

static menu_item_s settings_items[] = {
    {
        .label = "Toggle Bluetooth",
        .handler = toggle_bluetooth,
    }, {
        .label = "Turn screen off",
        .handler = turn_off_screen,
    }, {
        .label = "Flashlight",
        .handler = flashlight,
    }, {
        .label = "Credit",
        .handler = show_credit,
    }, {
        .label = sync_key_string,
        .handler = NULL,
    }
};

static void toggle_bluetooth(uint8_t item) {
    if(nsec_ble_toggle()) {
        nsec_status_set_ble_status(STATUS_BLUETOOTH_ON);
    }
    else {
        nsec_status_set_ble_status(STATUS_BLUETOOTH_OFF);
    }
}

static void show_credit(uint8_t item) {
    _state = SETTING_STATE_CREDIT;
    menu_close();
    gfx_fillRect(0, 8, 128, 56, BLACK);
    gfx_setCursor(0, 8);
    gfx_setTextBackgroundColor(WHITE, BLACK);
    gfx_puts("nsec 2017 badge team:");
    gfx_puts("Eric Tramblay\n");
    gfx_puts("@bvanheu\n");
    gfx_puts("@marc_etienne_");
    gfx_update();
}

static void turn_off_screen(uint8_t item) {
    menu_close();
    gfx_fillScreen(BLACK);
    gfx_update();
    _state = SETTING_STATE_SCREEN_OFF;
}

static void flashlight(uint8_t item) {
    menu_close();
    gfx_fillScreen(WHITE);
    gfx_update();
    _state = SETTING_STATE_FLASHLIGHT;
}

void nsec_setting_show(void) {
    char key[8];
    nsec_identity_get_unlock_key(key, sizeof(key));
    snprintf(sync_key_string, sizeof(sync_key_string), "Sync key: %s", key);
    gfx_fillRect(0, 8, 128, 65 - 8, BLACK);
    menu_init(0, 12, 128, 64 - 12, sizeof(settings_items) / sizeof(settings_items[0]), settings_items);
    nsec_controls_add_handler(setting_handle_buttons);
    _state = SETTING_STATE_MENU;
}

static void setting_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (_state) {
            case SETTING_STATE_MENU:
                _state = SETTING_STATE_CLOSED;
                menu_close();
                show_main_menu();
                break;

            case SETTING_STATE_FLASHLIGHT:
                gfx_fillScreen(BLACK);
            case SETTING_STATE_SCREEN_OFF:
                nsec_status_bar_ui_redraw();
            case SETTING_STATE_CREDIT:
                nsec_setting_show();
                break;

            default:
                break;
        }
    }
}
