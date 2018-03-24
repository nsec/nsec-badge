//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include "ble/nsec_ble.h"

#include <nrf.h>
#include <nrf_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf52.h>
#include <nrf52_bitfields.h>
#include <nordic_common.h>
#include <nrf_sdh.h>
#include <stdint.h>

#include "led_effects.h"
#include "logs.h"
#include "ble/ble_device.h"

#include "nrf_drv_power.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#define APP_TIMER_TIMEOUT     500

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info){
	NRF_LOG_ERROR("An error happened");
}

static void softdevice_init(void) {
    uint32_t err_code;
	err_code = nrf_sdh_enable_request();
	log_error_code("nrf_sdh_enable_request", err_code);
}

void init_devboard(){
	int leds[] = {17, 18, 19, 20};
	for(int i = 0; i < 4; i++)
		nrf_gpio_cfg_output(leds[i]);
	log_init();
}

/*
 * Initialize power module driver.
 *
 * Power driver has to be enabled before SoftDevice.
 */
void init_DCDC(void) {
    ret_code_t err_code;

    /*
     * Passing NULL as config will use the value of
     * POWER_CONFIG_DEFAULT_DCDCEN from sdk_config.h
     */
    err_code = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(err_code);
}

/*
 * Task timers
 */
APP_TIMER_DEF(m_heartbeat_timer_id);
static void heartbeat_timeout_handler(void *p_context) {
    // TODO notify ws2812fx
}

/*
 * Initialize the app timer library and heartbeat
 */
static void timers_init(void) {
    ret_code_t err_code;

    // Initialize the application timer module
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_heartbeat_timer_id ,
                APP_TIMER_MODE_REPEATED,
                heartbeat_timeout_handler);

    APP_ERROR_CHECK(err_code);
}

static void application_timers_start(void) {
    ret_code_t err_code;

    // Start application timers.
    //TODO confirm if 500ms is ok
    err_code = app_timer_start(m_heartbeat_timer_id,
                         APP_TIMER_TICKS(APP_TIMER_TIMEOUT /* ms */), NULL);
    APP_ERROR_CHECK(err_code);
}


int main() {
	init_devboard();
    init_DCDC();
    softdevice_init();
    timers_init();
    application_timers_start();
    create_ble_device("My BLE device");
    configure_advertising();
    config_dummy_service();
    start_advertising();
    while(1){
    	nrf_delay_ms(500);
    }
    return 0;
}
