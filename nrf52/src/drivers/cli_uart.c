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

#include "cli_uart.h"

#include <nrf_cli.h>
#include <nrf_cli_uart.h>

#include <boards.h>

NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 32, 32);
NRF_CLI_DEF(m_cli_uart, "nsec> ", &m_cli_uart_transport.transport, '\r', 4);

const nrf_cli_t *const p_cli_uart = &m_cli_uart;

/* Initialize Nordic's CLI module, using the UART transport. */

void cli_uart_init(void) {
    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;

    uart_config.pseltxd = PIN_nRF_TXD;
    uart_config.pselrxd = PIN_nRF_RXD;
    uart_config.hwfc = NRF_UART_HWFC_DISABLED;
    ret_code_t ret =
        nrf_cli_init(&m_cli_uart, &uart_config, /*use_colors=*/true,
                     /*log_backend=*/false, NRF_LOG_SEVERITY_INFO);

    APP_ERROR_CHECK(ret);

    ret = nrf_cli_start(&m_cli_uart);
    APP_ERROR_CHECK(ret);
}

/* Function that needs to be called periodically to do CLI stuff (read
   characters in, potentially call handlers).  */

void cli_uart_process(void) { nrf_cli_process(&m_cli_uart); }
