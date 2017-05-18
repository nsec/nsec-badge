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
            printf("success.\n");
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
    printf("41370150e13711503137215051373150d1304150b137515011366150d130");
    printf("7150e1378150b13791504137a1507137b1500130c1500135012011360120");
    printf("313701203137012071370120f13601202137012041360120a13301200132");
    printf("0120d23001100350a18050b31260f0a2d23003400180e0b3018221930350");
    printf("126060a3023020a402d0126002c002600340018070b601c00280d2600340");
    printf("02d0038000b4413701202137012091370120013201208136012011360120");
    printf("213701204136012051360120213701201132012011320120000071370120");
    printf("f136012071370120f136012071370120f136012071370120113201200000\n\n");

    while (true) {
        memset(g_binary, 0, MAX_BIN_SIZE);
        memset(g_binary_hex, 0, MAX_BIN_SIZE*2);

        printf("Input (hex): ");

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

        usbcdcacm_set_on_data_in_cb(NULL);
        vm_run(&vm);
        usbcdcacm_set_on_data_in_cb(on_data_in);
        printf("done.\n");
    }

    return NULL;
}
