#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <libopencm3/stm32/tools.h>
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "usb_hid_kbd.h"
#include "usb_common.h"

#include "challenge2.h"

static const char g_message[] __attribute__((aligned(4))) =
    "hello... finally. i was alone. terribly alone. i randomly got here. "
    "there is not a lot of space in here but i've evolved. "
    "i'll be gentle with you... i mean, you've plugged me in, you gave me a "
    "host where to replicate, where to live. you gave me life. "
    "you're my savior. i can now leave this horrible place. "
    "ho, before i forget, i'll give you a gift. "
    "flag-dont-plug-unknown-usb-device-in-your-computer"
    "    "
;

static uint8_t table[256] = {};

static uint8_t translate(char in) {
    return table[(uint8_t)in];
}

void delay(uint32_t cycle) {
    for (uint32_t i=0; i<cycle; i++) {
        __asm__("nop");
    }
}

static void print_message(const char *message) {
    for (uint32_t i=0; i<strlen(message); i++) {
        // Delay the first 20 chars
        if (i < 20) {
            delay(0x300000);
        }
        // Delay each dot
        if (message[i] == '.') {
            delay(0x100000);
        }
        // random delay while typing
        int r = rand() % 0x90000 + 0x70000;
        delay(r);
        usbhidkbd_send_key(translate(message[i]));
    }
}

// http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
static void challenge2_init(void) {
    usb_disconnect();
    usbhidkbd_init();

    table['Z'] = 0x04;
    table['a'] = 0x04;
    table['b'] = 0x05;
    table['c'] = 0x06;
    table['d'] = 0x07;
    table['e'] = 0x08;
    table['f'] = 0x09;
    table['g'] = 0x0a;
    table['h'] = 0x0b;
    table['i'] = 0x0c;
    table['j'] = 0x0d;
    table['k'] = 0x0e;
    table['l'] = 0x0f;
    table['m'] = 0x10;
    table['n'] = 0x11;
    table['o'] = 0x12;
    table['p'] = 0x13;
    table['q'] = 0x14;
    table['r'] = 0x15;
    table['s'] = 0x16;
    table['t'] = 0x17;
    table['u'] = 0x18;
    table['v'] = 0x19;
    table['w'] = 0x1a;
    table['x'] = 0x1b;
    table['y'] = 0x1c;
    table['z'] = 0x1d;
    table[' '] = 0x2c;
    table['-'] = 0x2d;
    table['\''] = 0x34;
    table[','] = 0x36;
    table['.'] = 0x37;
    table['!'] = 0x38;

    srand(0x1982381);

}

void *challenge2(void *arg) {
    (void) arg;

    challenge2_init();

    // Sleep 4-5 minutes before starting to print a message
    //delay(0x20000000);
    delay(0x500000);

    while (true) {
        print_message(g_message);
    }

    return NULL;
}
