//  Copyright (c) 2016
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "boards.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <nrf.h>
#include <nordic_common.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_nvic.h>
#include <app_util_platform.h>
#include <nrf_drv_saadc.h>
#include "logs.h"

#include "battery.h"


#define BATTERY_VOLTAGE_DIVIDER (4.7 / (4.7 + 10))
// Do not let the battery go under 1.8V
#define BATTERY_CHARGE_TRESHOLD 1800
// Even with the pulldown, the voltage is not 0 when the battery is not present.
#define NO_BATTERY_THRESHOLD 200
#define SAMPLES_IN_BUFFER 1

static volatile bool calibration_done = false;
static volatile uint32_t adc_value = 0;
static nrf_saadc_value_t conversion_buffer;

static uint16_t adc_in_millivolts(uint32_t adc_value);
static void saadc_callback(nrf_drv_saadc_evt_t const * p_event);
static void saadc_init();


void battery_refresh(void) {
	log_error_code("nrf_drv_saadc_sample", nrf_drv_saadc_sample());
}

uint16_t battery_get_voltage(){
	uint16_t voltage_at_adc = adc_in_millivolts(adc_value);
	return voltage_at_adc / BATTERY_VOLTAGE_DIVIDER;
}

bool battery_is_present() {
    return battery_get_voltage() > NO_BATTERY_THRESHOLD;
}

bool battery_is_undercharge() {
    return battery_get_voltage() < BATTERY_CHARGE_TRESHOLD;
}

bool battery_is_charging() {
    return nrf_gpio_pin_read(BATT_CHARGE) == 0;
}

bool battery_is_power_good() {
	return nrf_gpio_pin_read(BATT_PGOOD) == 0;
}

void battery_init() {
	nrf_gpio_cfg_input(BATT_CHARGE, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(BATT_PGOOD, NRF_GPIO_PIN_PULLUP);
    saadc_init();
}

static uint16_t adc_in_millivolts(uint32_t adc_value){
    const float adc_gain = 1.0F/6.0F;
    const int adc_resolution_in_bits = 8;
    const float reference_voltage = 600.0F;
    uint16_t voltage_in_millivolts = (uint16_t)(adc_value / ((adc_gain / reference_voltage)
    		* pow(2, adc_resolution_in_bits)));
    return voltage_in_millivolts;
}

static void calibrate_saadc(){
	if(nrf_drv_saadc_calibrate_offset() == NRF_SUCCESS)
		while(!calibration_done);
	else
		NRF_LOG_ERROR("SAADC is busy, cannot calibrate.");
}

static uint32_t register_buffer_for_next_conversion(){
	uint32_t error_code = nrf_drv_saadc_buffer_convert(&conversion_buffer, SAMPLES_IN_BUFFER);
	return error_code;
}

void saadc_callback(nrf_drv_saadc_evt_t const * p_event){
	if(p_event->type == NRF_DRV_SAADC_EVT_DONE){
		adc_value = p_event->data.done.p_buffer[0];
		register_buffer_for_next_conversion();
	}
	else if(p_event->type == NRF_DRV_SAADC_EVT_CALIBRATEDONE){
		calibration_done = true;
	}
}

void saadc_init(){
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
    channel_config.resistor_p = NRF_SAADC_RESISTOR_PULLDOWN;

    nrf_drv_saadc_config_t saadc_config;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_8BIT;
    saadc_config.low_power_mode = false;
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED;
    err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
    log_error_code("nrf_drv_saadc_init", err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    log_error_code("nrf_drv_saadc_channel_init", err_code);

    calibrate_saadc();
    register_buffer_for_next_conversion();
}
