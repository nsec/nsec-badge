// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "usb_cdc_acm.h"
#include "challenge1.h"
#include "challenge2.h"
#include "challenge3.h"
#include "challenge4.h"
#include "scheduler.h"

#include "shell.h"

#define BUFFER_SIZE 1024

static char line[BUFFER_SIZE+1] = {0};
static int line_index = 0;
static const char *prompt = "$ ";

static int shell_exec_command(char *cmd_name) {
    if (!strncmp(cmd_name, "chal1", 5)) {
        printf("Launching chal1...\n");

        struct task t = {
            .callback = &challenge1,
            .args = 0
        };
        scheduler_add_task(&t);
    }
    else if (!strncmp(cmd_name, "chal2", 5)) {
        printf("Launching chal2...\n");

        struct task t = {
            .callback = &challenge2,
            .args = 0
        };
        scheduler_add_task(&t);
        return 1;
    }
    else if (!strncmp(cmd_name, "chal3", 5)) {
        printf("Launching chal3...\n");
        struct task t = {
            .callback = &challenge3,
            .args = 0
        };
        scheduler_add_task(&t);
        return 1;
    }
    else if (!strncmp(cmd_name, "chal4", 5)) {
        printf("Launching chal4...\n");
        struct task t = {
            .callback = &challenge4,
            .args = 0
        };
        scheduler_add_task(&t);
        return 1;
    }

    return 0;
}


static void print_prompt(void) {
    printf(prompt);
    line_index = 0;
    line[0] = 0;
}

// new character to feed the shell
void shell_on_char_in(char c) {
    // <enter>
    if (c == '\n' || c == '\r') {
        printf("\n");

        if (*line) {
            if (!shell_exec_command(line)) {
                shell_print_help();
            }
            print_prompt();
            line_index = 0;
            line[0] = 0;
        }
        else {
            shell_print_help();
            print_prompt();
        }
    }
    // ctrl-c
    else if (c == 0x03) {
        printf("\r\n<interrupt>\r\n");
        print_prompt();
    }
    // backspace
    else if (c == 0x08 || c == 0x7f)
    {
        if (line_index > 0) {
            printf("\b \b");
            line_index--;
            line[line_index]=0;
        }
    }
    // help
    else if (c == '?') {
        shell_print_help();
        print_prompt();
        printf(line);
        line_index = strlen(line);
    }
    // any other char
    else {
        if (line_index < BUFFER_SIZE) {
            printf("%c", c);
            line[line_index++] = c;
            line[line_index] = '\0';
        }
    }
}

// print help for every commands
void shell_print_help(void) {
    printf("\n");
    printf("Available challenges\n");
    printf("--------------------\n");
    printf("chal1: computational device\n");
    printf("chal2: free'ing Rao's mind\n");
    printf("chal3: general purpose input/output\n");
    printf("chal4: redbox\n");
    printf("\n");
}

void shell_on_data_in(uint8_t *data, uint16_t size) {
    for (uint16_t i=0; i<size; i++) {
        shell_on_char_in(data[i]);
    }
}

void shell_init(void) {
    setbuf(stdout, NULL);

    usbcdcacm_init();
    usbcdcacm_set_on_data_in_cb(shell_on_data_in);
}

