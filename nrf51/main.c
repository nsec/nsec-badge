/*
 * Copyright (c) 2015, Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 * Copyright (c) 2016, Marc-Etienne M.Léveillé <marc.etienne.ml@gmail.com>
 * License: MIT (see LICENSE for details)
 */

#include "boards.h"

#include <app_util.h>
#include <app_scheduler.h>
#include <app_error.h>
#include <app_button.h>
#include <app_timer_appsh.h>
#include <app_gpiote.h>
#include <app_uart.h>

#include "ble/nsec_ble.h"

#include <softdevice_handler.h>

#include <nrf.h>
#include <nrf_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf51.h>
#include <nrf51_bitfields.h>
#include <nordic_common.h>

#include <stdbool.h>
#include <stdint.h>

#include "ssd1306.h"

#include "images/nsec_logo_bitmap.c"
#include "status_bar.h"
#include "menu.h"
#include "nsec_conf_schedule.h"
#include "nsec_settings.h"
#include "battery.h"
#include "touch_button.h"
#include "gfx_effect.h"
#include "led_effects.h"
#include "identity.h"
#include "exploit_challenge.h"
#include "nsec_nearby_badges.h"

static char g_device_id[10];

bool is_at_main_menu = false;

void wdt_init(void)
{
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CRV = 3*32768;             //ca 3 sek. timout
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
    NRF_WDT->TASKS_START = 1;           //Start the Watchdog timer
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
    static int error_displayed = 0;
    
    if(!error_displayed) {
        char error_msg[128];
        error_info_t * err_info = (error_info_t *) info;
        snprintf(error_msg, sizeof(error_msg), "%s:%u -> error 0x%08x\r\n", err_info->p_file_name, (unsigned int)err_info->line_num, (unsigned int)err_info->err_code);
        puts(error_msg);
        gfx_setCursor(0, 0);
        gfx_puts(error_msg);
        gfx_update();
        error_displayed = 1;
    }
    uint8_t count = 20;
    while (count > 0) {
        nsec_led_set_effect(count % 2 ? NSEC_LED_EFFECT_ALL_OFF : NSEC_LED_EFFECT_ALL_ON);
        nrf_delay_ms(250);
        count--;
    }

    NVIC_SystemReset(); 
}


/**
 * Callback function for asserts in the SoftDevice.
 * This function will be called in case of an assert in the SoftDevice.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
    app_error_handler(0xdeadbeef, line_num, p_file_name);
}


/**
 * Task timers
 */
// Green hearbeat
APP_TIMER_DEF(m_heartbeat_timer_id);
static void heartbeat_timeout_handler(void * p_context) {
    //if (flashlight_cmd_data.state == ST_INIT) {
    //    nrf_gpio_pin_set(LED_RGB_GREEN);
    //}

    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

/**
 * Init functions
 */
static void timers_init(void) {
    uint32_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, 16 /* APP_TIMER_OP_QUEUE_SIZE */, app_timer_evt_schedule);

    // Create timers.
    err_code = app_timer_create(&m_heartbeat_timer_id,
            APP_TIMER_MODE_REPEATED,
            heartbeat_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

void sys_evt_dispatch(uint32_t evt_id) {

}

static void softdevice_init(void) {
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_cfg = {
        .source = NRF_CLOCK_LF_SRC_RC,
        .rc_ctiv = 16,
        .rc_temp_ctiv = 2,
    };
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_cfg, false);

    // Register with the SoftDevice handler module for events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_start(void) {
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_heartbeat_timer_id, APP_TIMER_TICKS(500 /* ms */, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}

static void nsec_intro(void) {
    for(uint8_t noise = 128; noise <= 128; noise -= 8) {
        gfx_fillScreen(BLACK);
        gfx_drawBitmap(17, 11, nsec_logo_bitmap, nsec_logo_bitmap_width, nsec_logo_bitmap_height, WHITE);
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
    for(uint8_t noise = 0; noise <= 128; noise += 8) {
        gfx_fillScreen(BLACK);
        gfx_drawBitmap(17, 11, nsec_logo_bitmap, nsec_logo_bitmap_width, nsec_logo_bitmap_height, WHITE);
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
}

void open_animal_care(uint8_t item);
void open_conference_schedule(uint8_t item);
void open_settings(uint8_t item);

static menu_item_s main_menu_items[] = {
    {
        .label = "Conference schedule",
        .handler = open_conference_schedule,
    }, {
        .label = "Settings",
        .handler = open_settings,
    }
};

void open_conference_schedule(uint8_t item) {
    menu_close();
    is_at_main_menu = false;
    nsec_schedule_show_dates();
}

void open_settings(uint8_t item) {
    menu_close();
    is_at_main_menu = false;
    nsec_setting_show();
}

void show_main_menu(void) {
    for(uint8_t noise = 128; noise <= 128; noise -= 16) {
        gfx_fillScreen(BLACK);
        nsec_identity_draw();
        nsec_gfx_effect_addNoise(noise);
        gfx_update();
    }
    nsec_status_bar_ui_redraw();
    menu_init(0, 64-8, 128, 8, sizeof(main_menu_items) / sizeof(main_menu_items[0]), main_menu_items);
    is_at_main_menu = true;
}

/**
 * Main
 */
int main() {
    sprintf(g_device_id, "NSEC%04X", (uint16_t)(NRF_FICR->DEVICEID[1] % 0xFFFF));
    g_device_id[9] = '\0';

    softdevice_init();

    APP_SCHED_INIT(APP_TIMER_SCHED_EVT_SIZE /* EVENT_SIZE */, 12 /* QUEUE SIZE */);

    timers_init();
    APP_GPIOTE_INIT(2);

    nsec_led_init();

    ssd1306_init();
    touch_init();
    gfx_setTextBackgroundColor(WHITE, BLACK);

    application_timers_start();

    nsec_ble_init(g_device_id);
    nsec_ble_add_device_information_service(g_device_id, "NSEC 2017 Badge", NULL, NULL, NULL, NULL);

    nsec_vuln_init();

    nsec_identitiy_init();

    nsec_battery_manager_init();

    nsec_status_bar_init();
    nsec_status_set_name(g_device_id);
    nsec_status_set_badge_class("");
    nsec_status_set_ble_status(STATUS_BLUETOOTH_ON);

    nsec_intro();
    show_main_menu();

    nsec_identity_draw();
    nsec_nearby_badges_init();

    nsec_led_set_delay(100);
    nsec_led_set_effect(NSEC_LED_EFFECT_SPIN);

    while (true) {
        app_sched_execute();

        APP_ERROR_CHECK(sd_app_evt_wait());
    }

    return 0;
}
