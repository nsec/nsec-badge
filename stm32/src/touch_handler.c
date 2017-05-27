//
// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)
//

#include <stdio.h>
#include <unistd.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/tsc.h>
#include <libopencm3/stm32/spi.h>

#include "tsc_n.h"
#include "touch_handler.h"

static void tsc_conversation_completed_callback(void);

static void touch_clock_setup(void) {
    rcc_periph_clock_enable(RCC_TSC);
    rcc_periph_clock_enable(RCC_SPI1);
}

static void touch_tsc_setup(void) {
    tsc_set_high_charge_pulse_transfer(TSC_CTP_16);
    tsc_set_low_charge_pulse_transfer(TSC_CTP_16);
    tsc_set_spread_spectrum_prescaler(TSC_SSPSC_2);
    tsc_set_spread_spectrum_deviation(64);
    tsc_enable_spread_spectrum();
    tsc_set_pulse_generator_prescaler(TSC_PGPSC_64);
    tsc_set_max_count_value(TSC_MCV_255);
    tsc_set_io_mode_pushpull_low();

    // Clear interrupt flags
    tsc_enable_end_of_acquisition_interrupt();
    tsc_clear_end_of_acquisition_flag();

    // Clear Schmitt trigger
    // Excerpt from the doc:
    //  In order to improve the system immunity, the Schmitt trigger
    //  hysteresis of the GPIOs controlled by the TSC must be disabled by
    //  resetting the corresponding Gx_IOy bit in the
    tsc_clear_schmitt_trigger_hyst(TSC_GROUP_INDEX_1);
    tsc_clear_schmitt_trigger_hyst(TSC_GROUP_INDEX_2);

    // Enable/configure sampling/channel IOs
    tsc_enable_group(TSC_GROUP_INDEX_1);
    tsc_enable_group(TSC_GROUP_INDEX_2);
    tsc_enable_io_sampling(TSC_GROUP_INDEX_1, 1);
    tsc_enable_io_sampling(TSC_GROUP_INDEX_2, 4);
    tsc_enable_io_channel(TSC_GROUP_INDEX_1, 2);
    tsc_enable_io_channel(TSC_GROUP_INDEX_2, 1);

    nvic_enable_irq(NVIC_TSC_IRQ);

    tsc_enable();
}

static void touch_spi_setup(void) {
    spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_256);
    spi_set_master_mode(SPI1);
    // Enable SSI
    spi_set_nss_low(SPI1);
    spi_set_full_duplex_mode(SPI1);
    spi_set_clock_polarity_0(SPI1);
    spi_set_clock_phase_0(SPI1);
    // Disable SSM
    spi_disable_software_slave_management(SPI1);
    spi_send_msb_first(SPI1);
    spi_disable_crc(SPI1);
    spi_enable_ss_output(SPI1);
    spi_set_data_size(SPI1, SPI_CR2_DS_16BIT);
    // Enable NSS pulse
    SPI_CR2(SPI1) |= SPI_CR2_NSSP;

    spi_enable(SPI1);
}

static void touch_gpio_setup(void) {
    //
    // Setup GPIO for TSC
    //


    //  To allow the control of the sampling capacitor I/O by the TSC
    //  peripheral, the corresponding GPIO must be first set to alternate
    //  output open drain mode
    //  ...
    //  To allow the control of the channel I/O by the TSC peripheral,
    //  the corresponding GPIO must be first set to alternate output
    //  push-pull mode
#define GPIO_TSC_CHANNEL    GPIO1|GPIO2|GPIO3|GPIO4|GPIO5|GPIO6
#define GPIO_TSC_SAMPLING   GPIO0|GPIO7
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
            GPIO_TSC_SAMPLING|GPIO_TSC_CHANNEL);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LOW,
            GPIO_TSC_CHANNEL);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_LOW,
            GPIO_TSC_SAMPLING);
    gpio_set_af(GPIOA, GPIO_AF3, GPIO_TSC_CHANNEL|GPIO_TSC_SAMPLING);

    //
    // Setup GPIO for SPI1
    //
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO15);
    gpio_set_af(GPIOA, GPIO_AF0, GPIO15);
#define GPIO_SPI GPIO3|GPIO4|GPIO5
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_SPI);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_SPI);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO_SPI);
}

void tsc_isr(void) {
    if (tsc_get_end_of_acquisition_flag()) {
        tsc_clear_end_of_acquisition_flag();
        tsc_conversation_completed_callback();
    }

    if (tsc_get_max_count_error_flag()) {
        tsc_clear_max_count_error_flag();
    }
}

//
// TODO: this is hackish and should be cleaned up.
//
#define BUTTON_RELEASED 0
#define BUTTON_PRESSED 1
volatile uint8_t tsc_iochannel;
volatile uint8_t tsc_iochannel_status[6] = {0};

static void tsc_conversation_completed_callback() {
    uint32_t acquisition_value;

    // Group 1
    if (tsc_is_group_acquisition_completed(TSC_GROUP_INDEX_1)) {
        acquisition_value = tsc_group_read_counter(TSC_GROUP_INDEX_1);
        if (acquisition_value < 3) {
            // Button pressed
            if (tsc_iochannel_status[tsc_iochannel] == BUTTON_RELEASED) {
                tsc_iochannel_status[tsc_iochannel] = BUTTON_PRESSED;
                spi_send(SPI1, BUTTON_PRESSED << 8 | (tsc_iochannel+1));
            }
        }
        else {
            // Button released
            if (tsc_iochannel_status[tsc_iochannel] == BUTTON_PRESSED) {
                tsc_iochannel_status[tsc_iochannel] = BUTTON_RELEASED;
                spi_send(SPI1, BUTTON_RELEASED << 8 | (tsc_iochannel+1));
            }
        }
    }

    // Group 2
    if (tsc_is_group_acquisition_completed(TSC_GROUP_INDEX_2)) {
        acquisition_value = tsc_group_read_counter(TSC_GROUP_INDEX_2);
        if (acquisition_value <= 1) {
            // Button pressed
            if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_RELEASED) {
                tsc_iochannel_status[tsc_iochannel+3] = BUTTON_PRESSED;
                spi_send(SPI1, BUTTON_PRESSED << 8 | (tsc_iochannel+3+1));
            }
        }
        else {
            if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_PRESSED) {
                // Button released
                tsc_iochannel_status[tsc_iochannel+3] = BUTTON_RELEASED;
                spi_send(SPI1, BUTTON_RELEASED << 8 | (tsc_iochannel+3+1));
            }
        }
    }

    tsc_clear_io_channel(TSC_GROUP_INDEX_1);
    tsc_clear_io_channel(TSC_GROUP_INDEX_2);

    // Switch which button should be sampled
    switch (tsc_iochannel) {
        case 0:
            tsc_enable_io_channel(TSC_GROUP_INDEX_1, 2);
            tsc_enable_io_channel(TSC_GROUP_INDEX_2, 1);
            tsc_iochannel = 1;
            break;
        case 1:
            tsc_enable_io_channel(TSC_GROUP_INDEX_1, 3);
            tsc_enable_io_channel(TSC_GROUP_INDEX_2, 2);
            tsc_iochannel = 2;
            break;
        case 2:
            tsc_enable_io_channel(TSC_GROUP_INDEX_1, 4);
            tsc_enable_io_channel(TSC_GROUP_INDEX_2, 3);
            tsc_iochannel = 0;
            break;
    }

    tsc_discharge_io();

    tsc_start_acquisition();
}


void touch_init(void) {
    touch_clock_setup();
    touch_gpio_setup();
    touch_spi_setup();
    touch_tsc_setup();

    tsc_start_acquisition();
}
