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
    rcc_periph_clock_enable(RCC_TIM3);
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



void say_hello(void);

void say_hello(void) {
    printf("hello\n");
}

int main(void) {
    clock_setup();
    setup_tim3();

    touch_init();
    usart_init();

    say_hello();

    while (true) {
    }

    return 0;
}
