// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usb_cdc_acm.h"

#include "challenge4.h"
#include "challenge4_vm.h"

#define MAX_BIN_SIZE 1024

static uint8_t g_binary[MAX_BIN_SIZE] __attribute__((aligned(4)));
static char g_binary_hex[MAX_BIN_SIZE*2] __attribute__((aligned(4)));

static volatile bool g_launched = false;

static void hex_to_bin(uint8_t *output) {
    unsigned int temp = 0;
    for (uint32_t i=0,j=0; i<strlen(g_binary_hex); i+=2,j++) {
        sscanf(&g_binary_hex[i], "%02x", &temp);
        output[j] = (uint8_t)temp & 0xff;
    }
}

static void on_char_in(char c) {
    if (!g_launched) {
        if (c == '\n' || c == '\r') {
	    	g_launched = true;
            printf("\r\n");
        }
        else {
            g_binary_hex[strlen(g_binary_hex)] = c;
            printf("%c", c);
        }
    }
}

static void on_data_in(uint8_t *data, uint16_t size) {
    for (uint16_t i=0; i<size; i++) {
        on_char_in(data[i]);
    }
}

void *challenge4(void *arg) {
    (void)arg;
    usbcdcacm_set_on_data_in_cb(on_data_in);

    printf("In this country, we believe in security by obscurity.\n"
            "We made this obscure computing machine to hide our\n"
            "most worthy secret.\n"
            "No one has break it before!!!\n"
            "OUR SECRETS ARE SAFE\n\n\n");

    printf("Try this:\n");
    printf("0175012011760120317701203177012071770120f1760120217701204176");
    printf("0120a17301200172012061750190a17611904175219081753190817c4190");
    printf("4176519001776190817c7190a1768190417691906175a190c175b1900170");
    printf("c190d27001100390a15050d31230f0c2d27003800150f0d32162915111e0");
    printf("0390123060c3027030c402b0123002a002300380015080d601a00250d230");
    printf("038002b0035010d441770120217701209177012001720120817601201176");
    printf("012021770120417601205176012021770120117201201172012000007177");
    printf("0120f176012071770120f176012071770120f17601207177012011720120");
    printf("\n\n");

    while (true) {
        memset(g_binary, 0, MAX_BIN_SIZE);
        memset(g_binary_hex, 0, MAX_BIN_SIZE*2);

        printf("Machine input (hex): ");

        // Wait for user input...
        while (!g_launched);

        g_launched = false;

        struct vm vm;
        memset(vm.program, 0x00, sizeof(vm.program));
        memset(vm.memory, 0x00, sizeof(vm.memory));
        memset(vm.stack, 0x00, sizeof(vm.stack));

        hex_to_bin((uint8_t *)&vm.program);

        vm.status = STATUS_IS_RUNNING;
        vm.pc = 0x00;
        vm.sp = 0x00;
        vm.zf = 0x00;

        printf("Starting execution.\n");
        printf("------------8<------------\n");

        usbcdcacm_set_on_data_in_cb(NULL);
        vm_run(&vm);
        usbcdcacm_set_on_data_in_cb(on_data_in);
        printf("------------8<------------\n");
        printf("Execution finished.\n");
    }

    return NULL;
}
