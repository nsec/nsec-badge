//  Copyright (c) 2016
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "boards.h"

#include <stdint.h>
#include <stdbool.h>

#include <nrf.h>
#include <nordic_common.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_nvic.h>
#include <app_util_platform.h>
#include <nrf_drv_saadc.h>
#include "logs.h"

#include "battery.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS 600
#define ADC_GAIN 1.0/6.0
#define BATTERY_VOLTAGE_DIVIDER (4.7 / (4.7 + 10))
// Do not let the battery go under 1.8V
#define BATTERY_CHARGE_TRESHOLD         1800

static volatile bool calibration_done = false;
static volatile uint32_t adc_value = 0;
static uint16_t current_battery_voltage_in_millivolts;

/*
static volatile uint16_t m_batt_lvl_in_millivolts = 0;
static volatile bool m_batt_is_connected = false;

void ADC_IRQHandler(void) {
    if (NRF_ADC->EVENTS_END) {
        uint8_t adc_result;

        NRF_ADC->EVENTS_END = 0;
        adc_result = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP = 1;

        // A capacitor is missing on the PCB which makes the ADC not
        // really usable. We can't use ADC_RESULT_IN_MILLIVOLTS() here.
        m_batt_lvl_in_millivolts = adc_result;
    }
}
*/
void battery_refresh(void) {
	log_error_code("nrf_drv_saadc_sample", nrf_drv_saadc_sample());
}
/*
void battery_connect(void) {
    if (!m_batt_is_connected) {
        // Start the PSU by forcing a HIGH to the enable pin
        nrf_gpio_cfg_input(PSU_ENABLE, NRF_GPIO_PIN_PULLUP);
        m_batt_is_connected = true;
    }
}

void battery_disconnect(void) {
    if (m_batt_is_connected) {
        // Set the pin to HIGHZ, the physical pull down will disable the PSU. Once
        // reconnecting the USB cable, the USB voltage will take care of starting
        // the PSU
        nrf_gpio_cfg_input(PSU_ENABLE, NRF_GPIO_PIN_NOPULL);
        m_batt_is_connected = false;
    }
}
*/

static uint16_t adc_in_millivolts(uint32_t adc_value){
    float adc_gain = 1.0F/6.0F;
    int adc_resolution_bits = 8;
    float reference_voltage = 600.0F;
    uint16_t voltage_in_millivolts = (uint16_t)(adc_value / ((adc_gain / reference_voltage) * 256));
    return voltage_in_millivolts;
}

uint16_t battery_get_voltage(){
	uint16_t voltage_at_adc = adc_in_millivolts(adc_value);
	return voltage_at_adc / BATTERY_VOLTAGE_DIVIDER;
}

/*
bool battery_is_present() {
    return (m_batt_lvl_in_millivolts != 0);
}

bool battery_is_undercharge() {
    return (m_batt_lvl_in_millivolts < BATTERY_CHARGE_TRESHOLD);
}

bool battery_is_charging() {
    nrf_gpio_cfg_input(BATT_CHARGE, NRF_GPIO_PIN_NOPULL);
    return nrf_gpio_pin_read(BATT_CHARGE) != 0;
}
*/
#define SAMPLES_IN_BUFFER 1
volatile uint8_t state = 1;

//static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(0);
static nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];
//static nrf_ppi_channel_t     m_ppi_channel;

/*
void timer_handler(nrf_timer_event_t event_type, void * p_context){

}

void saadc_sampling_event_init(){
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    // setup m_timer for compare event every 400ms
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 400);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

    // setup ppi channel so that timer compare event is triggering sample task in SAADC
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}

void saadc_sampling_event_enable(){
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);

    APP_ERROR_CHECK(err_code);
}
*/
void saadc_callback(nrf_drv_saadc_evt_t const * p_event){

	if(p_event->type == NRF_DRV_SAADC_EVT_CALIBRATEDONE)
		calibration_done = true;
	else if(p_event->type == NRF_DRV_SAADC_EVT_DONE){
		nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
		adc_value = p_event->data.done.p_buffer[0];
	}
}

void saadc_init(){
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);

    nrf_drv_saadc_config_t saadc_config;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_8BIT;
    saadc_config.low_power_mode = false;
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED;
    err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
    log_error_code("nrf_drv_saadc_init", err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    log_error_code("nrf_drv_saadc_channel_init", err_code);

    //nrf_drv_saadc_calibrate_offset();
    //while(nrf_drv_saadc_calibrate_offset() != NRF_SUCCESS);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

}

void battery_init() {
    saadc_init();
    /*
	saadc_sampling_event_init();
	saadc_sampling_event_enable();
	*/
}
