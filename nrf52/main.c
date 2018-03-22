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
#include "battery.h"

#include "nrf_drv_power.h"

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info){
	NRF_LOG_ERROR("An error happened");
}

static void softdevice_init(void) {
    uint32_t err_code;
	err_code = nrf_sdh_enable_request();
	log_error_code("nrf_sdh_enable_request", err_code);
}

void test_neopixels(){
	if(nsec_neopixel_init() == -1)
		return;
	for(int i = 0; i < NEOPIXEL_COUNT; i++){
		nsec_set_pixel_color(i, 0, 0, 50);
	}
	while(1)
		nsec_neopixel_show();
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


void print_battery(){
	battery_refresh();
	int16_t voltage = battery_get_voltage();
	NRF_LOG_INFO("battery_voltage is %d", voltage);
}


int main() {
	/*
	init_devboard();
    init_DCDC();
    softdevice_init();
    create_ble_device("My BLE device");
    configure_advertising();
    config_dummy_service();
    start_advertising();
    */
	log_init();
	battery_init();
    while(1){
    	nrf_delay_ms(2000);
    	print_battery();
    }
    return 0;
}
