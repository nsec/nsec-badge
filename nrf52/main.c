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
#include <stdint.h>

#include "logs.h"
#include "ble/ble_device.h"

#include "buttons.h"
#include "led_effects.h"
#include "logs.h"
#include "timer.h"
#include "power.h"
#include "softdevice.h"
#include "ssd1306.h"

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info){
	NRF_LOG_ERROR("An error happened");
}

int main(void) {
    /*
     * Initialize base hardware
     */
    log_init();
    power_init();
    softdevice_init();
    timer_init();
    nsec_neoPixel_init();
    ssd1306_init();
    gfx_setTextBackgroundColor(WHITE, BLACK);
    buttons_init();

    /*
     * Initialize bluetooth stack
     */
    create_ble_device("My BLE device");
    configure_advertising();
    config_dummy_service();
    start_advertising();

    /*
     * Main loop
     */
    while(1) {
    	nrf_delay_ms(500);
    }

    return 0;
}
