//
// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)
//

#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "tsc_n.h"
#include "touch_handler.h"

#include <gdb_main.h>
#include <general.h>
#include <target.h>

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

    say_hello();

    // Blackmagic initialization
    platform_init();
    // Make a scan to set to SWD pins in a state where the debuggee can continue
    // to run and not stall in debug mode.
    adiv5_swdp_scan();

    // For some reason, it's very important to init the touch after the
    // Blackmagic initialization. The Blackmagic must change something that
    // breaks the SPI.
    touch_init();

    while (true) {
        gdb_main();
    }

    return 0;
}
