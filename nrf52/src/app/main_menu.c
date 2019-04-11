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
#include "menu.h"
#include "nsec_conf_schedule.h"
#include "nsec_games_menu.h"
#include "nsec_led_ble.h"
#include "nsec_led_pattern.h"
#include "nsec_settings.h"
#include "nsec_warning.h"
#include "status_bar.h"
#include "timer.h"
#include "utils.h"

enum main_menu_state { MAIN_MENU_STATE_CLOSED, MAIN_MENU_STATE_MENU };

static enum main_menu_state _state = MAIN_MENU_STATE_CLOSED;

static void main_handle_buttons(button_t button);

#ifdef NSEC_FLAVOR_CONF
void open_conference_schedule(uint8_t item) {
    menu_close();
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_schedule_show_dates();
}
#endif

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
        .label = "Challenges",
        .handler = open_challenges,
    },
    {
        .label = "LED pattern",
        .handler = open_led_pattern,
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

        default:
            break;
        }
    }
}
