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
#include "touch_handler.h"

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

    touch_init();

    say_hello();

    while (true) {
    }

    return 0;
}
