//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include <app_timer.h>
#include <nrf_gpio.h>

#include "timer.h"
#include "boards.h"
#include "drivers/battery.h"
#include "drivers/display.h"
#include "drivers/battery_manager.h"
#include "gfx_effect.h"
#include "gui.h"

uint64_t heartbeat_timeout_count = 0;

APP_TIMER_DEF(m_heartbeat_timer_id);
APP_TIMER_DEF(m_status_timer_id);
APP_TIMER_DEF(m_battery_status_timer_id);
APP_TIMER_DEF(m_battery_manager_timer_id);

static bool battery_event = false;
static bool battery_enable = false;

static
void _battery_manager_handler(void *p_context) {
    battery_manager_handler();
}

/*
 * Callback function when the heartbeat timeout expires
 */
static
void heartbeat_timeout_handler(void *p_context) {
    heartbeat_timeout_count++;
}

/*
 * Callback function when the status timeout expires
 */
static
void status_timeout_handler(void *p_context) {
   nrf_gpio_pin_toggle(PIN_LED_STATUS_1);
}

/*
 * Callback function when the battery status timeout expires
 */
static
void battery_status_timeout_handler(void *p_context) {
    battery_event = true;
}

void battery_status_process(void)
{
    if (battery_event && battery_enable) {
        battery_event = false;
        char msg[256];

        gfx_set_cursor(GEN_MENU_POS);
        gfx_set_text_background_color(HOME_MENU_BG_COLOR, DISPLAY_WHITE);

#ifdef BOARD_SPUTNIK
        snprintf(msg, sizeof(msg),
            "Battery status:\n"
            " Voltage: %04d mV\n"
            " Charging: %s\n"
            " USB plugged: %s\n",
            battery_get_voltage(),
            battery_is_charging() ? "Yes" : "No",
            battery_is_usb_plugged() ? "Yes" : "No");
#else
        snprintf(msg, sizeof(msg),
            "Battery status:\n"
            " Voltage: %04d mV\n",
            battery_get_voltage());
#endif

        gfx_puts(msg);
        gfx_update();
    }
}

/*
 * Initialize the app timer library and heartbeat
 */
void timer_init(void) {
    ret_code_t err_code;

    // Initialize the application timer module
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create the heartbeat timer
    err_code = app_timer_create(&m_heartbeat_timer_id,
                APP_TIMER_MODE_REPEATED,
                heartbeat_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start the heartbeat timer
    err_code = app_timer_start(m_heartbeat_timer_id,
                APP_TIMER_TICKS(HEARTBEAT_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg_output(PIN_LED_STATUS_1);

    // Create the status timer
    err_code = app_timer_create(&m_status_timer_id,
                APP_TIMER_MODE_REPEATED,
                status_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Start the status timer
    err_code = app_timer_start(m_status_timer_id,
                APP_TIMER_TICKS(STATUS_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);

    // Create the battery status timer
    err_code = app_timer_create(&m_battery_status_timer_id,
                APP_TIMER_MODE_REPEATED,
                battery_status_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

/*
 * Get the elapsed time since startup in milliseconds
 */
uint64_t get_current_time_millis(void) {
    return (app_timer_cnt_get() / APP_TIMER_CLOCK_FREQ) +
           (HEARTBEAT_TIMER_TIMEOUT * heartbeat_timeout_count);
}

void start_battery_status_timer(void) {
    ret_code_t err_code;

    err_code = app_timer_start(m_battery_status_timer_id,
                APP_TIMER_TICKS(BATTERY_STATUS_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);

    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);

    battery_enable = true;
    battery_event = true;
    battery_status_timeout_handler(NULL);
}

void stop_battery_status_timer(void) {
    ret_code_t err_code;

    err_code = app_timer_stop(m_battery_status_timer_id);
    APP_ERROR_CHECK(err_code);

    battery_enable = false;
}

void start_battery_manage_timer(void) {
    ret_code_t err_code = app_timer_create(&m_battery_manager_timer_id,
                APP_TIMER_MODE_REPEATED,
                _battery_manager_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_battery_manager_timer_id,
                APP_TIMER_TICKS(BATTERY_MANAGER_TIMER_TIMEOUT_MS), NULL);
    APP_ERROR_CHECK(err_code);
}
