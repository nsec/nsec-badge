#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/flash.h>

#include "tsc_n.h"
#include "touch_handler.h"
#include "shell.h"
#include "scheduler.h"
#include "usb_cdc_acm.h"

static void clock_setup(void) {
    rcc_clock_setup_in_hsi_out_48mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
}

void enable_read_protect(void);
void enable_read_protect(void) {
    if ((FLASH_OBR & FLASH_OBR_RDPRT) != FLASH_OBR_RDPRT_L1) {
        flash_unlock();
        flash_erase_option_bytes();
        flash_program_option_bytes(0x1ffff800, 0x55f0);
        FLASH_CR |= FLASH_CR_OBL_LAUNCH;
        flash_lock();
        // Wait for system reboot
        while (true);
    }
}

int main(void) {
    clock_setup();
    enable_read_protect();

    touch_init();
    scheduler_init();

    usbcdcacm_init();

    delay(0x8000);

    shell_init();

    while (true) {
        scheduler_execute();
    }

    return 0;
}


void hard_fault_handler(void) {
    while(1);
    }
