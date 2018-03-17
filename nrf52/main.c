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
		nsec_set_pixel_color(i, 255, 255, 255);
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


int main() {
	init_devboard();
    softdevice_init();
    create_ble_device("My BLE device");
    configure_advertising();
    config_dummy_service();
    start_advertising();
    while(1){
    	nrf_delay_ms(500);
    }
    return 0;
}
