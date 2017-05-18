#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "tsc_n.h"
#include "touch_handler.h"
#include "usart.h"
#include "shell.h"
#include "scheduler.h"
#include "usb_cdc_acm.h"

static void clock_setup(void) {
    rcc_clock_setup_in_hsi_out_48mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
}

int main(void) {
    clock_setup();
    touch_init();
    scheduler_init();

    usbcdcacm_init();

    shell_init();
    delay(0x8000);

    while (true) {
        scheduler_execute();
    }

    return 0;
}


void hard_fault_handler(void) {
    while(1);
    }
