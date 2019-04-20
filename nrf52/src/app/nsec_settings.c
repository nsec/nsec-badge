//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nrf.h>
#include <nordic_common.h>
#include <stdio.h>
#include "home_menu.h"
#include "main_menu.h"
#include "gui.h"
#include "nsec_settings.h"
#include "nsec_led_settings.h"
#include "menu.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "ble/nsec_ble.h"
#include "status_bar.h"
#include "drivers/controls.h"
#include "identity.h"
#include "drivers/ws2812fx.h"
#include "persistency.h"
#include "timer.h"

//static void toggle_bluetooth(uint8_t item);
static void show_credit(uint8_t item);
static void turn_off_screen(uint8_t item);
static void show_led_settings(uint8_t item);
static void show_display_brightness(uint8_t item);
static void save_brightness(uint8_t item);
static void toggle_flashlight(uint8_t item);
static void show_member_details(uint8_t item);
static void setting_handle_buttons(button_t button);

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_CREDIT,
    SETTING_STATE_CREDIT_DETAILS,
    SETTING_STATE_SCREEN_OFF,
    SETTING_STATE_FLASHLIGHT,
    SETTING_STATE_BATTERY,
    SETTING_STATE_DISPLAY
};

static enum setting_state _state = SETTING_STATE_CLOSED;

static void setting_handle_buttons(button_t button);

static char sync_key_string[] = "Sync key: XXXX";

static menu_item_s settings_items[] = {
    {
        .label = "Led settings",
        .handler = show_led_settings,
    }, {
        .label = "Display brightness",
        .handler = show_display_brightness,
    }, /*{
        .label = "Toggle Bluetooth",
        .handler = toggle_bluetooth,
    }, */{
        .label = "Turn screen off",
        .handler = turn_off_screen,
    }, {
        .label = "Flashlight",
        .handler = toggle_flashlight,
    }, {
        .label = "Credit",
        .handler = show_credit,
    }, {
        .label = sync_key_string,
        .handler = NULL,
    }
};

static menu_item_s members_items[] = {
    {
        .label = "Eric Tremblay",
        .handler = show_member_details,
    }, {
        .label = "Michael Jeanson",
        .handler = show_member_details,
    }, {
        .label = "Nicolas Aubry",
        .handler = show_member_details,
    }, {
        .label = "Simon Marchi",
        .handler = show_member_details,
    }, {
        .label = "Thomas Dupuy",
        .handler = show_member_details,
    }, {
        .label = "Victor Nikulshin",
        .handler = show_member_details,
    }, {
        .label = "Yannick Lamarre",
        .handler = show_member_details,
    },
};

static menu_item_s brightness_items[] = {
    {
        .label = "Min",
        .handler = save_brightness,
    }, {
        .label = "Low",
        .handler = save_brightness,
    }, {
        .label = "Medium",
        .handler = save_brightness,
    }, {
        .label = "High",
        .handler = save_brightness,
    }, {
        .label = "Max",
        .handler = save_brightness,
    }
};

static void show_member_details(uint8_t item) {
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    gfx_set_cursor(GEN_MENU_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    _state = SETTING_STATE_CREDIT_DETAILS;

    switch (item) {
        //Line   |                     | 21 character
        // There are 7 lines under the title
        case 0: //Eric Tremblay
        gfx_puts("VP Badge nsec 2019\n");
        gfx_puts("habscup@gmail.com\n");
        gfx_puts("Hardware\n");
        gfx_puts("Software\n");
        break;

        case 1: //Michael Jeanson
        gfx_puts("Badge drone nsec 2019\n");
        gfx_puts("Michael Jeanson\n");
        gfx_puts("mjeanson@gmail.com\n");
        gfx_puts("@mjeanson\n");
        gfx_puts("Software\n");
        break;

        case 2: // Nicolas Aubry
        show_credit(4);
        break;

        case 3: //Simon Marchi
        show_credit(4);
        break;

        case 4: //Thomas Dupuy
        show_credit(4);
        break;

        case 5: //Victor Nikulshin
        show_credit(4);
        break;

        case 6: //Yannick Lamarre
        show_credit(4);
        break;
    }
    gfx_update();
}

static void show_led_settings(uint8_t item) {
    menu_close();
    _state = SETTING_STATE_CLOSED;
    nsec_show_led_settings();
}

static void save_brightness(uint8_t item)
{
    uint8_t b = (item==0) ? 1 : item*25;
    display_set_brightness(b);
    update_stored_display_brightness(b);
}

static void draw_display_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 25;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "DISPLAY");
    draw_title(&title);
}

static void show_display_brightness(uint8_t item)
{
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_display_title();
    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
        ARRAY_SIZE(brightness_items), brightness_items,
        HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    _state = SETTING_STATE_DISPLAY;
}

/*static void toggle_bluetooth(uint8_t item) {
    if(nsec_ble_toggle()) {
        nsec_status_set_ble_status(STATUS_BLUETOOTH_ON);
    }
    else {
        nsec_status_set_ble_status(STATUS_BLUETOOTH_OFF);
    }
}*/

static void draw_credit_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 25;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "CREDIT");
    draw_title(&title);
}

static void show_credit(uint8_t item) {
    _state = SETTING_STATE_CREDIT;
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_credit_title();
    gfx_set_cursor(GEN_MENU_POS);
    gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_puts("nsec 2019 badge team:");
    menu_init(GEN_MENU_POS_X, GEN_MENU_POS_Y + 8, GEN_MENU_WIDTH,
        GEN_MENU_HEIGHT - 8, ARRAY_SIZE(members_items), members_items,
        HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    gfx_update();
}

static void draw_battery_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 25;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "BATTERY");
    draw_title(&title);
}

void show_battery_status(void) {
    _state = SETTING_STATE_BATTERY;
    draw_battery_title();
    menu_close();
    start_battery_status_timer();
    nsec_controls_add_handler(setting_handle_buttons);
}

static void toggle_flashlight(uint8_t item) {
    _state = SETTING_STATE_FLASHLIGHT;
    menu_close();
    gfx_fill_screen(DISPLAY_WHITE);
    gfx_update();
    setMode_WS2812FX(FX_MODE_STATIC);
    setBrightness_WS2812FX(255);
    setColor_packed_WS2812FX(WHITE);
}

static void turn_off_screen(uint8_t item) {
    menu_close();
    display_set_brightness(0);
    _state = SETTING_STATE_SCREEN_OFF;
}

void nsec_setting_show(void) {
    char key[8];

    nsec_identity_get_unlock_key(key, sizeof(key));
#ifdef NSEC_HIDE_SYNC_KEY_IN_SETTINGS
    snprintf(sync_key_string, sizeof(sync_key_string), "Sync key: %s", "XXXX");
#else
    snprintf(sync_key_string, sizeof(sync_key_string), "Sync key: %s", key);
#endif

    draw_settings_title();

    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
        ARRAY_SIZE(settings_items), settings_items,
        HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    nsec_controls_add_handler(setting_handle_buttons);
    _state = SETTING_STATE_MENU;
}

static void setting_handle_buttons(button_t button)
{
    if (button == BUTTON_BACK) {
        switch (_state) {
            case SETTING_STATE_MENU:
                _state = SETTING_STATE_CLOSED;
                menu_close();
                show_home_menu(HOME_STATE_SETTINGS);
                break;

            case SETTING_STATE_CREDIT_DETAILS:
                show_credit(4);
                break;

            case SETTING_STATE_BATTERY:
                stop_battery_status_timer();
                // Battery status was moved to the main menu
                // this is a hack to make it work with minimal
                // code change
                _state = SETTING_STATE_CLOSED;
                menu_close();
                show_main_menu();
                break;

            case SETTING_STATE_FLASHLIGHT:
                load_led_settings();
                nsec_status_bar_ui_redraw();
                draw_home_menu_bar();
                draw_settings_title();

                // no break
            case SETTING_STATE_SCREEN_OFF:
                display_set_brightness(get_stored_display_brightness());
                // no break
            case SETTING_STATE_DISPLAY:
            case SETTING_STATE_CREDIT:
                nsec_setting_show();
                break;

            default:
                break;
        }
    }
}
