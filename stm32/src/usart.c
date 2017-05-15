#include <stdio.h>
#include <unistd.h>

#include "usart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define USART_CONSOLE USART1

int _write(int file, char *ptr, int len);

static void usart_clock_setup(void) {
    rcc_periph_clock_enable(RCC_USART1);
    //rcc_periph_clock_enable(RCC_USART3);
}

static void usart_gpio_setup(void) {
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

void usart_init(void) {
    usart_clock_setup();
    usart_gpio_setup();
    usart_setup();
}
