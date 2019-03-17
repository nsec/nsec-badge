/*
 * Copyright 2019 Michael Jeanson <mjeanson@gmail.com>
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

#include <app_error.h>
#include <nrf_drv_twi.h>

#include <boards.h>
#include <drivers/sao.h>

static const nrf_drv_twi_t sao_twi_master =
    NRF_DRV_TWI_INSTANCE(CONF_SAO_TWI_INST);

#ifdef PIN_SAO_SCL
void sao_init(void) {
    ret_code_t err_code;

    const nrf_drv_twi_config_t sao_twi_config = {
        .scl = PIN_SAO_SCL,
        .sda = PIN_SAO_SDA,
        .frequency = NRF_TWI_FREQ_400K,
        .interrupt_priority = TWI_DEFAULT_CONFIG_IRQ_PRIORITY,
        .clear_bus_init = false};

    err_code = nrf_drv_twi_init(&sao_twi_master, &sao_twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&sao_twi_master);
}
#else
void sao_init(void) {}
#endif
