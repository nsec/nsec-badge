//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <nrf.h>
#include <stdio.h>

#include "drivers/controls.h"
#include "drivers/display.h"

#include "application.h"
#include "challenges.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "identity.h"
#include "persistency.h"
#include "menu.h"
#include "text_box.h"
#include "nsec_conf_schedule.h"
#include "nsec_games_menu.h"
#include "nsec_led_pattern.h"
#include "nsec_settings.h"
#include "nsec_warning.h"
#include "status_bar.h"
#include "timer.h"
#include "utils.h"
#include "app_flashlight.h"

enum main_menu_state {
    MAIN_MENU_STATE_CLOSED,
    MAIN_MENU_STATE_MENU,
    MAIN_MENU_STATE_BADGE_INFO,
    MAIN_MENU_CLI_INFO
};

static enum main_menu_state _state = MAIN_MENU_STATE_CLOSED;

static void main_handle_buttons(button_t button);

static char identity_string[] = "Id: Citizen #XXXXXXX";
static char ble_id_string[] = "BLE id: NSECXXXX";

static const char *cli_info =
    "The badge provides a command line interface that is accessible via the "
    "USB port.\n"
    "Connect the badge to a PC and explore it. To access the CLI you can "
    "use the serial tool of your choice and then connect to the second tty or "
    "COM spawned on your PC.\n"
    "The serial parameter are: "
    "baudrate = 115200\n"
    "databits = 8\n"
    "stopbit = 1\n"
    "no parity\n\n"
    "Example with picocom: \n"
    "picocom /dev/ttyACM1 -b 115200\n";

static struct text_box_config config = {
    GEN_MENU_POS_X,
    22,
    160,
    GEN_MENU_HEIGHT,
    HOME_MENU_BG_COLOR,
    DISPLAY_WHITE
};

#ifdef NSEC_FLAVOR_CONF
void open_conference_schedule(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_schedule_show_dates();
}
#endif

static void draw_cli_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 45;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "CLI");
    draw_title(&title);
}

void open_challenges(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_challenges_show();
}

void open_led_pattern(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_led_pattern_show();
}

void open_warning(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_warning_show();
}

void open_battery_status(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    show_battery_status();
}

void open_games_menu(uint8_t item)
{
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_games_menu_show();
}

static void open_flashlight(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    application_set(app_flashlight);
}

void show_badge_cli_info(uint8_t item)
{
    menu_close();
    _state = MAIN_MENU_CLI_INFO;
    gfx_fill_rect(0, 0, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_cli_title();
    text_box_init(cli_info, &config);
}

static menu_item_s badge_info_items[] = {
    {
        .label = "Badge CLI",
        .handler = show_badge_cli_info,
    }, {
        .label = identity_string,
        .handler = NULL,
    }, {
        .label = ble_id_string,
        .handler = NULL,
    }
};

static void show_badge_info(uint8_t item)
{
    menu_close();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);

    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(badge_info_items), badge_info_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    _state = MAIN_MENU_STATE_BADGE_INFO;
}

static menu_item_s main_menu_items[] = {
#ifdef NSEC_FLAVOR_CONF
    {
        .label = "Conference schedule",
        .handler = open_conference_schedule,
    },
#endif
    {
        .label = "Games",
        .handler = open_games_menu,
    },
    {
        .label = "LED pattern",
        .handler = open_led_pattern,
    },
    {
        .label = "Badge info",
        .handler = show_badge_info,
    },
    {
        .label = "Flashlight",
        .handler = open_flashlight,
    },
    {
        .label = "Battery status",
        .handler = open_battery_status,
    },
    {
        .label = "Battery Warning",
        .handler = open_warning,
    }};

void show_main_menu(void) {
#if defined(NSEC_HARDCODED_BLE_DEVICE_ID)
    sprintf(ble_id_string, "%.8s", NSEC_STRINGIFY(NSEC_HARDCODED_BLE_DEVICE_ID));
#else
    sprintf(ble_id_string, "BLE id: NSEC%04X", (uint16_t)(NRF_FICR->DEVICEID[1] % 0xFFFF));
#endif

    sprintf(identity_string, "Id: %s", get_stored_identity());

    draw_main_menu_title();
    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(main_menu_items), main_menu_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
    nsec_controls_add_handler(main_handle_buttons);
    _state = MAIN_MENU_STATE_MENU;
}

static void main_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (_state) {
        case MAIN_MENU_STATE_MENU:
            _state = MAIN_MENU_STATE_CLOSED;
            menu_close();
            show_home_menu(HOME_STATE_MENU);
            break;
        case MAIN_MENU_STATE_BADGE_INFO:
            _state = MAIN_MENU_STATE_MENU;
            show_main_menu();
            break;
        case MAIN_MENU_CLI_INFO:
            _state = MAIN_MENU_STATE_BADGE_INFO;
            draw_main_menu_title();
            redraw_home_menu_burger_selected();
            show_badge_info(2);
            break;
        default:
            break;
        }
    }
}
