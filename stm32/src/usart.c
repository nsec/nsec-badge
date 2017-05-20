#include <stdio.h>
#include <unistd.h>

#include "usart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define USART_CONSOLE USART1

int _write(int file, char *ptr, int len);

static void usart_clock_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_USART1);
}

static void usart_gpio_setup(void) {
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO6);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO7);
}

static void usart_setup(void) {
    usart_set_baudrate(USART_CONSOLE, 115200);
    usart_set_databits(USART_CONSOLE, 8);
    usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
    usart_set_stopbits(USART_CONSOLE, USART_CR2_STOP_1_0BIT);
    usart_set_mode(USART_CONSOLE, USART_MODE_TX);
    usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);
    usart_enable(USART_CONSOLE);
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
