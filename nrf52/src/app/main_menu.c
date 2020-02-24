//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <nrf.h>
#include <stdio.h>

#include "main_menu.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "drivers/buttons.h"
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

static menu_t g_menu;

#ifdef NSEC_FLAVOR_CONF
static void open_conference_schedule(uint8_t item) {
    _state = MAIN_MENU_STATE_CLOSED;
    nsec_schedule_show_dates();
}
#endif

static void main_menu_page_redraw(void)
{
    draw_main_menu_title();
    menu_ui_redraw_all(&g_menu);
}

static void open_led_pattern(uint8_t item)
{
    nsec_led_pattern_show();
}

static void show_badge_info(uint8_t item)
{
}

static void open_warning(uint8_t item)
{
    nsec_warning_show();
}

static void open_battery_status(uint8_t item)
{
    //    show_battery_status();
}

static void open_games_menu(uint8_t item)
{
    show_ui_page(&games_menu_page, NULL);
}

static void open_flashlight(uint8_t item) {
    application_set(app_flashlight);
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

static bool main_menu_page_handle_button(button_t button)
{
    if (button == BUTTON_BACK) {
        return true;
    }

    menu_button_handler(&g_menu, button);

    return false;
}

static void main_menu_page_init(void *data)
{
    menu_init(&g_menu, GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(main_menu_items), main_menu_items, HOME_MENU_BG_COLOR,
              DISPLAY_WHITE);
}

const ui_page main_menu_page = {
    .init = main_menu_page_init,
    .redraw = main_menu_page_redraw,
    .handle_button = main_menu_page_handle_button,
};
