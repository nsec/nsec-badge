/*
 * Copyright 2016-2017 Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 *           2016-2017 Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
 *           2018 NicolasAubry <nicolas.aubry@hotmail.com>
 *           2018 Michael Jeanson <mjeanson@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

#include "battery.h"

// Reference voltage is 0.6V
#define ADC_REF_VOLTAGE_MV   600

#define ADC_RES_10BITS 1024

// Compensate for the 1 in 6 gain
#define ADC_PRE_SCALING_COMPENSATION 6

// Physical voltage divider
#define BATTERY_VOLTAGE_DIVIDER (4.7 / (4.7 + 10.0))

#define ADC_RESULT_IN_MILLIVOLTS(ADC_VALUE) \
(\
 ((((ADC_VALUE) * ADC_REF_VOLTAGE_MV) / ADC_RES_10BITS) * ADC_PRE_SCALING_COMPENSATION) / BATTERY_VOLTAGE_DIVIDER \
)

// Even with the pulldown, the voltage is not 0 when the battery is not present.
#define NO_BATTERY_THRESHOLD_MV 200

// Number of samples to average
#define SAMPLES_IN_BUFFER 3

static volatile bool saadc_calibration_done = false;
static volatile uint16_t m_batt_lvl_in_millivolts = 0;
static nrf_saadc_value_t m_buffer_pool[2][SAMPLES_IN_BUFFER];

void battery_refresh(void) {
    ret_code_t err_code;

    err_code = nrf_drv_saadc_sample();
    APP_ERROR_CHECK(err_code);
}

uint16_t battery_get_voltage() {
    return m_batt_lvl_in_millivolts;
}

bool battery_is_present() {
    return (m_batt_lvl_in_millivolts > NO_BATTERY_THRESHOLD_MV);
}

#ifdef BOARD_SPUTNIK
bool battery_is_charging() {
    /*
     * RT9525GQW pin CHG, active low.
     */
    return (nrf_gpio_pin_read(PIN_BATT_CHARGE) == 0);
}

bool battery_is_usb_plugged() {
    /*
     * RT9525GQW pin PGOOD, active low.
     */
    return (nrf_gpio_pin_read(PIN_BATT_PGOOD) == 0);
}
#endif

static
void calibrate_saadc() {
    ret_code_t err_code;

    /*
     * Trigger the ADC offset calibration.
     */
    err_code = nrf_drv_saadc_calibrate_offset();
    APP_ERROR_CHECK(err_code);

    /*
     * The calibration is non-blocking, wait for it to complete.
     */
    while(true) {
        if (saadc_calibration_done) {
            break;
        }
        nrf_delay_ms(10);
    }
}

/*
 * Callback function to process SAADC events.
 */
void saadc_callback(nrf_drv_saadc_evt_t const *p_event) {
    switch (p_event->type) {
    /*
     * SAADC calibration is completed.
     */
    case NRF_DRV_SAADC_EVT_CALIBRATEDONE:
        saadc_calibration_done = true;
        break;

    /*
     * A new sample is available in the buffer, convert it to millivolts.
     */
    case NRF_DRV_SAADC_EVT_DONE:
    {
        uint16_t average = 0;
        ret_code_t err_code;

    /*
     * Convert the raw adc value to millivolts and compute the average
     * value of the samples.
     */
        for (int i = 0; i < SAMPLES_IN_BUFFER; i++) {
            average += ADC_RESULT_IN_MILLIVOLTS(p_event->data.done.p_buffer[i]) / SAMPLES_IN_BUFFER;
    }
        m_batt_lvl_in_millivolts = average;

    /*
     * Switch buffer for next sampling.
     */
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
    break;
    }

    case NRF_DRV_SAADC_EVT_LIMIT:
    break;
    }
}

void battery_init() {
    ret_code_t err_code;

#ifdef BOARD_SPUTNIK
    nrf_gpio_cfg_input(PIN_BATT_CHARGE, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_BATT_PGOOD, NRF_GPIO_PIN_PULLUP);
#endif

    /*
     * Initialize the saadc driver, passing NULL as config will use the
     * default values from sdk_config.h of :
     *  SAADC_CONFIG_RESOLUTION
     *  SAADC_CONFIG_OVERSAMPLE
     *  SAADC_CONFIG_LP_MODE
     *  SAADC_CONFIG_IRQ_PRIORITY
     */
    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

    /*
     * Create a default channel configuration in single ended mode
     * for the battery voltage pin.
     */
    static nrf_saadc_channel_config_t battery_channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);

    /*
     * The adc channel resistor is pulled down to ground.
     */
    battery_channel_config.resistor_p = NRF_SAADC_RESISTOR_PULLDOWN;

    /*
     * Configure and enable channel 0 with the battery channel config.
     */
    err_code = nrf_drv_saadc_channel_init(0, &battery_channel_config);
    APP_ERROR_CHECK(err_code);

    /*
     * Calibrate the saadc offset.
     */
    calibrate_saadc();

    /*
     * Setup double buffering.
     */
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);

    /*
     * Collect first battery voltage value.
     */
    battery_refresh();
}
