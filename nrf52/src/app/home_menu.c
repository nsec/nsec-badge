//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "application.h"
#include "home_menu.h"
#include "drivers/battery_manager.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/nsec_storage.h"
#include "gfx_effect.h"
#include "gui.h"
#include "main_menu.h"
#include "menu.h"
#include "nsec_settings.h"
#include "status_bar.h"
#include <string.h>

#include "images/neurosoft_logo_bitmap.h"
#include "images/external/nsec_logo_color_bitmap.h"
#include "images/settings_off_bitmap.h"
#include "images/settings_on_bitmap.h"

bool is_at_home_menu = false;

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static void home_menu_handle_buttons(button_t button);

static enum home_state _state = HOME_STATE_CLOSED;

static void draw_burger_menu_icon(int16_t x, int16_t y, uint16_t color) {
    gfx_fill_rect(x, y, 3, 3, color);
    gfx_fill_rect(x, y + 6, 3, 3, color);
    gfx_fill_rect(x, y + 12, 3, 3, color);

    gfx_fill_rect(x + 6, y, 12, 3, color);
    gfx_fill_rect(x + 6, y + 6, 12, 3, color);
    gfx_fill_rect(x + 6, y + 12, 12, 3, color);
}

static void draw_cursor(void) {
    if (_state == HOME_STATE_MENU) {
        gfx_fill_triangle(HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y,
                          HOME_MENU_POS_X + 7, SETTINGS_MENU_CUR_Y + 5,
                          HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y + 10,
                          HOME_MENU_BG_COLOR);

        gfx_fill_triangle(HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y,
                          HOME_MENU_POS_X + 7, BURGER_MENU_CUR_Y + 5,
                          HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y + 10,
                          DISPLAY_RED);
    } else {
        gfx_fill_triangle(HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y,
                          HOME_MENU_POS_X + 7, BURGER_MENU_CUR_Y + 5,
                          HOME_MENU_POS_X + 1, BURGER_MENU_CUR_Y + 10,
                          HOME_MENU_BG_COLOR);

        gfx_fill_triangle(HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y,
                          HOME_MENU_POS_X + 7, SETTINGS_MENU_CUR_Y + 5,
                          HOME_MENU_POS_X + 1, SETTINGS_MENU_CUR_Y + 10,
                          DISPLAY_RED);
    }
}

void draw_title(struct title *title) {
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, GEN_MENU_POS_Y,
                  title->bg_color);

    gfx_set_cursor(title->pos_x, title->pos_y);
    gfx_set_text_size(2);
    gfx_set_text_background_color(title->text_color, title->bg_color);
    gfx_puts(title->text);
    gfx_set_text_size(1);
}

void draw_settings_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = SETTINGS_MENU_TITLE_X;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "SETTINGS");
    draw_title(&title);
}

void draw_main_menu_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = BURGER_MENU_TITLE_X;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "MENU");
    draw_title(&title);
}

void draw_home_menu_bar(void)
{
    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT,
                  HOME_MENU_BG_COLOR);

    draw_burger_menu_icon(BURGER_MENU_POS, DISPLAY_WHITE);

    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_off_bitmap,
                          HOME_MENU_BG_COLOR);
}

void redraw_home_menu_burger_selected(void)
{
    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2,
                  DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS_X, HOME_MENU_POS_Y + (HOME_MENU_HEIGHT / 2),
                  HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2, HOME_MENU_BG_COLOR);

    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_off_bitmap,
                          HOME_MENU_BG_COLOR);

    draw_burger_menu_icon(BURGER_MENU_POS, HOME_MENU_BG_COLOR);

}

static void draw_home_menu(void) {
    draw_home_menu_bar();

    draw_cursor();

    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_BLACK);

    // TODO :
    // -    Replace by logo without Red circle and animate the
    //      red circle
#ifdef NSEC_FLAVOR_CTF
    gfx_draw_16bit_bitmap(NEUROSOFT_LOGO_POS, &neurosoft_logo_bitmap,
                          DISPLAY_BLACK);
#else
    display_draw_16bit_ext_bitmap(NSEC_LOGO_POS, &nsec_logo_color_bitmap,
                                   DISPLAY_BLACK);
    gfx_set_cursor(CONF_STR_POS);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_puts("Conference");
#endif
}

void show_home_menu(enum home_state state) {
    _state = state;

    is_at_home_menu = true;
    draw_home_menu();
}

static void open_burger_menu(void) {
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2,
                  DISPLAY_WHITE);

    draw_burger_menu_icon(BURGER_MENU_POS, HOME_MENU_BG_COLOR);

    draw_main_menu_title();

    _state = HOME_STATE_CLOSED;
    is_at_home_menu = false;
    show_main_menu();
}

static void open_settings_menu(void) {
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS_X, HOME_MENU_POS_Y + (HOME_MENU_HEIGHT / 2),
                  HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2, DISPLAY_WHITE);

    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_on_bitmap,
                          DISPLAY_WHITE);

    draw_settings_title();

    _state = HOME_STATE_CLOSED;
    is_at_home_menu = false;
    nsec_setting_show();
}

static void home_menu_handle_buttons(button_t button) {
    if (!is_at_home_menu) {
        return;
    }

    switch (button) {
    case BUTTON_BACK:
        if (_state == HOME_STATE_MENU || _state == HOME_STATE_SETTINGS) {
            // Close menu (show intro ???); or maybe nothing
        } else if (_state == HOME_STATE_MENU_SELECTED) {
            _state = HOME_STATE_MENU;
            draw_home_menu();
        } else {
            _state = HOME_STATE_SETTINGS;
            draw_home_menu();
        }
        break;

    case BUTTON_DOWN:
        if (_state == HOME_STATE_MENU) {
            _state = HOME_STATE_SETTINGS;
            draw_cursor();
        } else if (_state == HOME_STATE_SETTINGS) {
            _state = HOME_STATE_MENU;
            draw_cursor();
        }
        break;

    case BUTTON_UP:
        if (_state == HOME_STATE_MENU) {
            _state = HOME_STATE_SETTINGS;
            draw_cursor();
        } else if (_state == HOME_STATE_SETTINGS) {
            _state = HOME_STATE_MENU;
            draw_cursor();
        }

        break;

    case BUTTON_ENTER:
        if (_state == HOME_STATE_MENU) {
            _state = HOME_STATE_MENU_SELECTED;
            open_burger_menu();
        } else {
            _state = HOME_STATE_SETTINGS_SELECTED;
            open_settings_menu();
        }
        break;

    default:
        break;
    }
}

void home_menu_application(void (*service_callback)()) {
    menu_handler_init();
    nsec_status_bar_init();
    nsec_status_set_ble_status(STATUS_BLUETOOTH_ON);
    nsec_battery_manager_init();
    show_home_menu(HOME_STATE_MENU);

    nsec_controls_add_handler(home_menu_handle_buttons);

    while (application_get() == home_menu_application) {
        battery_manager_process();
        service_callback();
    }

    nsec_controls_suspend_handler(home_menu_handle_buttons);
}
