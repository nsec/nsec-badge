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
#include <softdevice_handler.h>

#include "battery.h"

// Reference voltage is 1.2V
#define ADC_REF_VOLTAGE_IN_MILLIVOLTS   1200
// Pre-scaling compensation: 1/(2/3)
#define ADC_PRE_SCALING_COMPENSATION    1.5
// Physical voltage divider
#define BATTERY_VOLTAGE_DIVIDER         ((10e6+4.7e6)/4.7e6)
// Do not let the battery go under 3.2 V
#define BATTERY_CHARGE_TRESHOLD         3200

#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
    (((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION))*BATTERY_VOLTAGE_DIVIDER

static volatile uint16_t m_batt_lvl_in_milli_volts = 0;
static volatile bool m_batt_is_connected = false;

void ADC_IRQHandler(void) {
    if (NRF_ADC->EVENTS_END) {
        uint8_t adc_result;

        NRF_ADC->EVENTS_END = 0;
        adc_result = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP = 1;

        m_batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result);
    }
}

void battery_refresh(void) {
    NRF_ADC->TASKS_START = 1;
}

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

uint16_t battery_get_voltage() {
    return m_batt_lvl_in_milli_volts;
}

bool battery_is_undercharge() {
    return (m_batt_lvl_in_milli_volts < BATTERY_CHARGE_TRESHOLD);
}

bool battery_is_charging() {
    nrf_gpio_cfg_input(BATT_CHARGE, NRF_GPIO_PIN_NOPULL);
    return nrf_gpio_pin_read(BATT_CHARGE) != 0;
}

void battery_init() {
    uint32_t err_code;

    // Interrupt on END event
    NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;

    // 8 bit resolution
    NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos)
        // 2/3 prescaling
        | (ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling << ADC_CONFIG_INPSEL_Pos)
        // Pin where the battery voltage is
        | (ADC_CONFIG_PSEL_AnalogInput3 << ADC_CONFIG_PSEL_Pos)
        // Use internal 1.2V reference voltage for conversion
        | (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)
        // No external reference
        | (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);

    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;

    nrf_delay_us(1000);

    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->TASKS_START = 1;

    // Enable the ADC interrupt
    err_code = sd_nvic_ClearPendingIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_LOW);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_EnableIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);
}
