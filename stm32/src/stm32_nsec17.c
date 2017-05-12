#include <stdio.h>
#include <unistd.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/tsc.h>
#include <libopencm3/stm32/spi.h>

#include "tsc_n.h"

#define USART_CONSOLE USART1

int _write(int file, char *ptr, int len);
void *print_hello(void *args);

static void clock_setup(void) {
    rcc_clock_setup_in_hsi_out_48mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USART1);
    //rcc_periph_clock_enable(RCC_USART3);
    rcc_periph_clock_enable(RCC_TIM3);
    rcc_periph_clock_enable(RCC_TSC);
    rcc_periph_clock_enable(RCC_SPI1);
}

static void tsc_setup(void) {
    // 16 cycles
    tsc_set_high_charge_pulse_transfer(0xf);
    tsc_set_low_charge_pulse_transfer(0xf);
    tsc_set_spread_spectrum_deviation(64);
    // DIV1
    tsc_set_spread_spectrum_prescaler(1);
    tsc_enable_spread_spectrum();
    // DIV64
    tsc_set_pulse_generator_prescaler(6);
    // 511
    tsc_set_max_count_value(1);
    tsc_set_io_mode_pushpull_low();

    // Clear interrupt flag
    tsc_enable_end_of_acquisition_interrupt();
    tsc_clear_end_of_acquisition_flag();

    tsc_clear_schmitt_trigger_hyst(1);

    // Enable groups
    tsc_enable_group(1);
    tsc_enable_group(2);
    tsc_enable_io_sampling(1, 1);
    tsc_enable_io_sampling(2, 4);
    tsc_enable_io_channel(1, 2);
    tsc_enable_io_channel(2, 1);

    nvic_enable_irq(NVIC_TSC_IRQ);

    tsc_enable();
}

static void spi_setup(void) {
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

static void usart_setup(void) {
	// Setup USART1 parameters. (uart on header)
	usart_set_baudrate(USART_CONSOLE, 115200);
	usart_set_databits(USART_CONSOLE, 8);
	usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
	usart_set_stopbits(USART_CONSOLE, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART_CONSOLE, USART_MODE_TX);
	usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);
    usart_enable(USART_CONSOLE);

	// Setup USART3 parameters. (uart to NRF51)
	//usart_set_baudrate(USART3, 115200);
	//usart_set_databits(USART3, 8);
	//usart_set_parity(USART3, USART_PARITY_NONE);
	//usart_set_stopbits(USART3, USART_CR2_STOP_1_0BIT);
	//usart_set_mode(USART3, USART_MODE_TX_RX);
	//usart_set_flow_control(USART3, USART_FLOWCONTROL_RTS_CTS);
    //usart_enable_rx_interrupt(USART3);
    //nvic_enable_irq(NVIC_USART3_4_IRQ);
	//usart_enable(USART3);
}

static void gpio_setup(void) {
    //
    // Setup GPIO for USART1
    //
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
	gpio_set_af(GPIOB, GPIO_AF0, GPIO6);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO7);

    //// Setup GPIO pins for USART3 TX.
    //gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
    //gpio_set_af(GPIOB, GPIO_AF4, GPIO10);
    //// Setup GPIO pins for USART3 RX.
    //gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11);
    //gpio_set_af(GPIOB, GPIO_AF4, GPIO11);
    //// Setup GPIO pins for USART3 CTS.
    //gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
    //gpio_set_af(GPIOB, GPIO_AF4, GPIO13);
    //// Setup GPIO pins for USART3 RTS
    //gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14);
    //gpio_set_af(GPIOB, GPIO_AF4, GPIO14);

    //
    // Setup GPIO for TSC
    //
#define GPIO_TSC GPIO0|GPIO1|GPIO2|GPIO3|GPIO4|GPIO5|GPIO6|GPIO7
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_TSC);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_TSC);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO0);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO7);
    gpio_set_af(GPIOA, GPIO_AF3, GPIO_TSC);

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

static volatile int t3ovf;

// TIM3 - ping every ms
void tim3_isr(void) {
	TIM_SR(TIM3) &= ~TIM_SR_UIF;
	if (t3ovf++ > 100) {
        // TODO - handle scheduler here

        // Reset the timer overflow flag.
		t3ovf = 0;
	}
}

void tsc_conversation_completed_callback(void);
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

void tsc_conversation_completed_callback() {
    // Group 1
    if (tsc_is_group_acquisition_completed(1)) {
        // Button pressed
        if (tsc_iochannel_status[tsc_iochannel] == BUTTON_RELEASED) {
            tsc_iochannel_status[tsc_iochannel] = BUTTON_PRESSED;
            spi_send(SPI1, BUTTON_PRESSED << 8 | (tsc_iochannel+1));
        }
    }
    else if (tsc_iochannel_status[tsc_iochannel] == BUTTON_PRESSED) {
        // Button released
        tsc_iochannel_status[tsc_iochannel] = BUTTON_RELEASED;
        spi_send(SPI1, BUTTON_RELEASED << 8 | (tsc_iochannel+1));
    }

    // Group 2
    if (tsc_is_group_acquisition_completed(2)) {
        // Button pressed
        if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_RELEASED) {
            tsc_iochannel_status[tsc_iochannel+3] = BUTTON_PRESSED;
            spi_send(SPI1, BUTTON_PRESSED << 8 | (tsc_iochannel+3));
        }
    }
    else if (tsc_iochannel_status[tsc_iochannel+3] == BUTTON_PRESSED) {
        // Button released
        tsc_iochannel_status[tsc_iochannel+3] = BUTTON_RELEASED;
        spi_send(SPI1, BUTTON_RELEASED << 8 | (tsc_iochannel+3));
    }

    tsc_clear_io_channel(1);
    tsc_clear_io_channel(2);

    // Switch which button should be sampled
    switch (tsc_iochannel) {
        case 0:
            tsc_enable_io_channel(1, 2);
            tsc_enable_io_channel(2, 1);
            tsc_iochannel = 1;
            break;
        case 1:
            tsc_enable_io_channel(1, 3);
            tsc_enable_io_channel(2, 2);
            tsc_iochannel = 2;
            break;
        case 2:
            tsc_enable_io_channel(1, 4);
            tsc_enable_io_channel(2, 3);
            tsc_iochannel = 0;
            break;
    }

    tsc_discharge_io();

    tsc_start_acquisition();
}

static void setup_tim3(void) {
    rcc_periph_reset_pulse(RST_TIM3);
    // Set the timer to 1ms interrupt
    // 48 MHz / 10 kHz -> prescaler = 4800 Hz
    timer_set_prescaler(TIM3, 4800);
    // 10kHz / 10 ticks = 1 kHz overflow = 1ms overflow interrupts
    timer_set_period(TIM3, 10);

    nvic_enable_irq(NVIC_TIM3_IRQ);
    timer_enable_update_event(TIM3);
    timer_enable_irq(TIM3, TIM_DIER_UIE);
    timer_enable_counter(TIM3);
}

// Enable the use of printf and other stdio's function.
int _write(int file, char *ptr, int len) {
    int i;
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (i = 0; i < len; i++) {
            if (ptr[i] == '\n') {
                usart_send_blocking(USART_CONSOLE, '\r');
            }
            usart_send_blocking(USART_CONSOLE, ptr[i]);
        }
        return i;
    }
    //errno = EIO;
    return -1;
}

void say_hello(void);

void say_hello(void) {
    printf("hello\n");
}

int main(void) {
    clock_setup();
    gpio_setup();
    usart_setup();
    setup_tim3();
    spi_setup();
    tsc_setup();

    tsc_start_acquisition();

    say_hello();

    while (true) {
    }

    return 0;
}
