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

#include <nordic_common.h>
#include <app_util_platform.h>
#include <nrf_drv_uart.h>
#include <string.h>
#include "boards.h"

#define UART_DEFAULT_CONFIG_HWFC NRF_UART_HWFC_DISABLED
#define UART_DEFAULT_CONFIG_PARITY NRF_UART_PARITY_EXCLUDED
#define UART_DEFAULT_CONFIG_BAUDRATE NRF_UART_BAUDRATE_38400
#define UART_DEFAULT_CONFIG_IRQ_PRIORITY APP_IRQ_PRIORITY_LOW

static nrf_drv_uart_t instance = NRF_DRV_UART_INSTANCE(0);

/*
 * Initialize the uart in blocking mode, send function will return once the
 * transfer is complete.
 */
ret_code_t uart_init()
{
	nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
	config.pseltxd = PIN_nRF_TXD;
	config.pselrxd = PIN_nRF_RXD;
	config.pselcts = PIN_nRF_CTS;
	config.pselrts = PIN_nRF_RTS;

	return nrf_drv_uart_init(&instance, &config, NULL);
}

ret_code_t uart_send(const char *data)
{
    return nrf_drv_uart_tx(&instance, (unsigned char*)data, strlen(data));
}
