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

#include "uart.h"
#include "boards.h"
#include <app_util_platform.h>
#include <nordic_common.h>
#include <nrf_drv_uart.h>
#include <stdarg.h>
#include <string.h>

static nrf_drv_uart_t instance = NRF_DRV_UART_INSTANCE(0);

/*
 * Initialize the uart in blocking mode, send function will return once the
 * transfer is complete.
 */
ret_code_t uart_init() {
    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.pseltxd = PIN_nRF_TXD;
    config.pselrxd = PIN_nRF_RXD;
    config.pselcts = PIN_nRF_CTS;
    config.pselrts = PIN_nRF_RTS;

    ret_code_t ret = nrf_drv_uart_init(&instance, &config, NULL);
    if (ret != NRF_SUCCESS)
        return ret;

    nrf_drv_uart_rx_enable(&instance);

    return NRF_SUCCESS;
}

/* Write the LEN bytes pointed to by DATA to the UART.  */

ret_code_t uart_send(const uint8_t *data, size_t len) {
    return nrf_drv_uart_tx(&instance, data, len);
}

/* Send the null-terminated string pointed to by STR.  */

ret_code_t uart_puts(const char *str) {
    return uart_send((const uint8_t *)str, strlen(str));
}

/* For the string printf-style and send it.  */

ret_code_t uart_printf(const char *fmt, ...) {
    static char str[100];
    va_list args;
    va_start(args, fmt);
    int len = vsprintf(str, fmt, args);
    ret_code_t ret = uart_send((uint8_t *)str, len);
    va_end(args);
    return ret;
}

/* Read one byte from the UART rx fifo (blocking if necessary), place it in
 * *OUT.  Return true on success, false otherwise.  */

bool uart_read(uint8_t *out) { return nrf_drv_uart_rx(&instance, out, 1); }

/* Clear the UART errors flags.  */

void uart_clear_errors() { nrf_drv_uart_errorsrc_get(&instance); }
