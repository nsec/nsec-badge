/*
 * Copyright 2018 Eric Tremblay <habscup@gmail.com>
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
#include <nrf_soc.h>

#include "power.h"

/*
 * Initialize power module driver.
 *
 * The power driver has to be enabled before the SoftDevice.
 */
void power_init(void) {
    ret_code_t err_code;

    /*
     * Passing NULL as config will use the value of
     * POWER_CONFIG_DEFAULT_DCDCEN from sdk_config.h
     */
    err_code = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(err_code);
}

void power_manage(void) {
	uint32_t err_code = sd_app_evt_wait();
	APP_ERROR_CHECK(err_code);
}
