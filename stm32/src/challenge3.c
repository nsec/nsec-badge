#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <libopencm3/stm32/tools.h>
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "challenge3.h"

static void challenge3_init(void) {
    challenge3_init();

    #define GPIO_CHALLENGE3 GPIO8|GPIO9|GPIO10
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO_CHALLENGE3);
    gpio_clear(GPIOA, GPIO_CHALLENGE3);
}

void *challenge3(void *arg) {
    (void) arg;
    challenge3_init();

    printf("This device can be unlocked when receiving a code on some of its\n"
           "GPIOs. Which one you're asking? Have you found the schematics yet?\n"
           "That might not help...\n");

    uint16_t meh = 0;
    while (meh != GPIO8) {
        meh = gpio_get(GPIOA, GPIO8);
    };
    printf("gpio 1/3.\n");

    while (gpio_get(GPIOA, GPIO9) != GPIO9);
    printf("gpio 2/3..\n");

    while (gpio_get(GPIOA, GPIO10) != GPIO10);
    printf("gpio 3/3...now the game can start!\n");

    #include "challenge3_solution.h"

    printf("gratz! here is your flag: FLAG_THIS_WENT_BETTER_THAN_EXPECTED\n");

    return NULL;
}
