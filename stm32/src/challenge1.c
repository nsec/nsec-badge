//
// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)
//

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "usb_cdc_acm.h"

#include "challenge1.h"

static const char *g_password = "timingiskey";
static char g_input_password[256] = {0};

static volatile bool g_newline = false;
static volatile bool g_done = false;

static void delay(uint32_t cycle) {
    for (uint32_t i=0; i<cycle; i++) {
        __asm__("nop");
    }
}

static bool strslowcmp(char *input_password) {
    (void) input_password;

    bool equal = true;

    for (uint8_t i=0; i<strlen(g_password); i++) {
        if (g_password[i] != g_input_password[i]) {
            // Quit when we find the first difference
            equal = false;
            break;
        }

        // Introduce a *long enough* delay to simulate computation
        delay(0x100000);
    }

    return equal;
}


static void on_char_in(char c) {
    if (!g_newline) {
        if (c == '\n' || c == '\r') {
            g_newline = true;
            printf("\r\n");
        }
        g_input_password[strlen(g_input_password)] = c;
        printf("%c", c);
    }
}

static void on_data_in(uint8_t *data, uint16_t size) {
    for (uint16_t i=0; i<size; i++) {
        on_char_in(data[i]);
    }
}

static void challenge1_init(void) {
    printf("You've been given this very special embedded device that contains\n");
    printf("a precious flag. Unfortunately, it's protected by a computational\n");
    printf("intensive algorithm. Will you unlock the flag?\n");

    usbcdcacm_set_on_data_in_cb(on_data_in);
}

void *challenge1(void *arg) {
    (void) arg;

    challenge1_init();

    while (!g_done) {
        printf("Password: ");

        while (!g_newline) {
            // Wait for a newline while we wait for the password...
        }

        g_done = strslowcmp(g_input_password);

        if (!g_done) {
            printf("try again.\r\n");
            g_newline = 0;
            memset(g_input_password, 0, 256);
        }
    }

    printf("well done! Now reboot the chip...\r\n");

    while (true);

    return NULL;
}

