#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "tsc_n.h"
#include "touch_handler.h"
#include "usart.h"

static void clock_setup(void) {
    rcc_clock_setup_in_hsi_out_48mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
}

void say_hello(void);

void say_hello(void) {
    printf("hello\n");
}

int main(void) {
    clock_setup();

    touch_init();
    usart_init();

    say_hello();

    while (true) {
    }

    return 0;
}
