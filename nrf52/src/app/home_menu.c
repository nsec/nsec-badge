//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "home_menu.h"
#include "FreeRTOS.h"
#include "app_screensaver.h"
#include "application.h"
#include "drivers/battery_manager.h"
#include "drivers/buttons.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include "main_menu.h"
#include "menu.h"
#include "nsec_settings.h"
#include "persistency.h"
#include "queue.h"
#include "status_bar.h"
#include "timer.h"

#include <string.h>

#include "images/neurosoft_logo_bitmap.h"
#include "images/neurosoft_logo_a_1_bitmap.h"
#include "images/neurosoft_logo_a_2_bitmap.h"
#include "images/neurosoft_logo_a_3_bitmap.h"
#include "images/neurosoft_logo_a_4_bitmap.h"
#include "images/neurosoft_logo_a_5_bitmap.h"
#include "images/neurosoft_logo_a_6_bitmap.h"
#include "images/neurosoft_logo_a_7_bitmap.h"
#include "images/neurosoft_logo_a_8_bitmap.h"
#include "images/neurosoft_logo_a_9_bitmap.h"
#include "images/neurosoft_logo_a_10_bitmap.h"
#include "images/neurosoft_logo_a_11_bitmap.h"
#include "images/neurosoft_logo_a_12_bitmap.h"
#include "images/neurosoft_logo_a_13_bitmap.h"
#include "images/neurosoft_logo_a_14_bitmap.h"
#include "images/neurosoft_logo_a_15_bitmap.h"
#include "images/settings_off_bitmap.h"
#include "images/settings_on_bitmap.h"

static bool _is_at_home_menu = false;

static uint16_t gfx_width;
static uint16_t gfx_height;

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

static void draw_home_menu_logo_animation(void)
{
    static uint8_t frame = 0;

    if (_is_at_home_menu) {
        switch (frame) {
        case 0:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_1_bitmap, 0);
            break;

        case 3:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_2_bitmap, 0);
            break;

        case 6:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_3_bitmap, 0);
            break;

        case 9:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_4_bitmap, 0);
            break;

        case 12:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_5_bitmap, 0);
            break;

        case 15:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_6_bitmap, 0);
            break;

        case 18:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_7_bitmap, 0);
            break;

        case 21:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_8_bitmap, 0);
            break;

        case 24:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_9_bitmap, 0);
            break;

        case 27:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_10_bitmap, 0);
            break;

        case 30:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_11_bitmap, 0);
            break;

        case 33:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_12_bitmap, 0);
            break;

        case 36:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_13_bitmap, 0);
            break;

        case 39:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_14_bitmap, 0);
            break;

        case 42:
            gfx_draw_16bit_bitmap(NEUROSOFT_ANIMATION_POS,
                                  &neurosoft_logo_a_15_bitmap, 0);
            break;
        }

        frame = frame >= 45 ? 0 : frame + 1;
    } else {
        frame = 0;
    }
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

#ifdef NSEC_FLAVOR_CONF
#include "images/external/conf/nsec_logo_color_bitmap.h"

static void draw_home_menu(void)
{
    draw_home_menu_bar();
    draw_cursor();

    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_BLACK);

    display_draw_16bit_ext_bitmap(NSEC_LOGO_POS, &nsec_logo_color_bitmap,
                                   DISPLAY_BLACK);

    gfx_set_cursor(CONF_STR_POS);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_set_text_size(1);
    gfx_puts("Conference");
}
#else
static void draw_home_menu(void)
{
    draw_home_menu_bar();
    draw_cursor();

    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_BLACK);

    gfx_draw_16bit_bitmap(NEUROSOFT_LOGO_POS, &neurosoft_logo_bitmap,
                          DISPLAY_BLACK);
}
#endif

void show_home_menu(enum home_state state) {
    _state = state;

    _is_at_home_menu = true;
    gfx_width = gfx_get_screen_width();
    gfx_height = gfx_get_screen_height();
    draw_home_menu();
}

static void open_burger_menu(void) {
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS, HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2,
                  DISPLAY_WHITE);

    draw_burger_menu_icon(BURGER_MENU_POS, HOME_MENU_BG_COLOR);

    draw_main_menu_title();

    show_main_menu();
    draw_home_menu();
}

static void open_settings_menu(void) {
    gfx_fill_rect(0, 0, gfx_width - HOME_MENU_WIDTH, gfx_height, DISPLAY_WHITE);

    gfx_fill_rect(HOME_MENU_POS_X, HOME_MENU_POS_Y + (HOME_MENU_HEIGHT / 2),
                  HOME_MENU_WIDTH, HOME_MENU_HEIGHT / 2, DISPLAY_WHITE);

    gfx_draw_16bit_bitmap(SETTINGS_MENU_POS, &settings_on_bitmap,
                          DISPLAY_WHITE);

    draw_settings_title();

    _state = HOME_STATE_CLOSED;
    _is_at_home_menu = false;
    nsec_setting_show();
}

static void home_menu_handle_buttons(button_t button) {
    /* Reset the screensaver timeout on each button event */
    // screensaver_reset();

    /* Don't handle the buttons if we are in a submenu */
    // if (!_is_at_home_menu) {
    //    return;
    //}

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
            open_burger_menu();
        } else {
            open_settings_menu();
        }
        break;

    default:
        break;
    }
}

void home_menu_application(void)
{
    /* Reset the screensaver timer when we start the home menu */
    // screensaver_reset();

    // menu_handler_init();
    // nsec_status_bar_init();
    // nsec_status_set_ble_status(get_stored_ble_is_enabled());
    // nsec_battery_manager_init();
    show_home_menu(HOME_STATE_MENU);

    uint32_t last_ms = get_current_time_millis();

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, 25);
        if (ret == pdTRUE) {
            home_menu_handle_buttons(btn);
        } else {
            /* The receive timed out. */
        }

#ifdef NSEC_FLAVOR_CTF
        /* Animate the logo. */
        uint32_t this_ms = get_current_time_millis();
        if (this_ms - last_ms > 20) {
            draw_home_menu_logo_animation();
            last_ms = this_ms;
        }
#endif
    }
}

bool is_at_home_menu(void)
{
    return _is_at_home_menu;
}
