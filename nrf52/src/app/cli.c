/*
 * Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "cli.h"

#include <drivers/cli_uart.h>

/* The "prout" command. */

static void do_prout(const nrf_cli_t *p_cli, size_t argc, char **argv) {
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_MAGENTA,
                    " ____                  _   _\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_MAGENTA,
                    "|  _ \\ _ __ ___  _   _| |_| |\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_MAGENTA,
                    "| |_) | '__/ _ \\| | | | __| |\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_MAGENTA,
                    "|  __/| | | (_) | |_| | |_|_|\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_MAGENTA,
                    "|_|   |_|  \\___/ \\__,_|\\__(_)\r\n");
}

NRF_CLI_CMD_REGISTER(prout, NULL, "Does prout.", do_prout);

/* Initialize the command-line interface module.  */

void cli_init(void) { cli_uart_init(); }

/* Function that needs to be called periodically to do CLI stuff (read
   characters in, potentially call handlers).  */

void cli_process(void) { cli_uart_process(); }
