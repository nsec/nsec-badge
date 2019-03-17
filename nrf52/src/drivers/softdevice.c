/*
 * Copyright 2015-2017, Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 *           2016-2017, Marc-Etienne M.Léveillé <marc.etienne.ml@gmail.com>
 *           2018, Michael Jeanson <mjeanson@gmail.com>
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

#include <nrf_delay.h>

#include "softdevice.h"

NRF_SDH_STATE_OBSERVER(m_state_observer, 0) = {.handler = state_evt_handler,
                                               .p_context = NULL};

void state_evt_handler(nrf_sdh_state_evt_t state, void *p_context) {
    switch (state) {
    case NRF_SDH_EVT_STATE_ENABLE_PREPARE:
        // SoftDevice is about to be enabled.
        break;
    case NRF_SDH_EVT_STATE_ENABLED:
        // SoftDevice is now enabled.
        break;
    case NRF_SDH_EVT_STATE_DISABLE_PREPARE:
        // SoftDevice is about to be disabled.
        break;
    case NRF_SDH_EVT_STATE_DISABLED:
        // SoftDevice is now disabled.
        break;
    }
}

void softdevice_init(void) {
    uint32_t err_code;

    // Attempt to enable the SoftDevice
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Wait for softdevice to initialize
    while (true) {
        err_code = nrf_sdh_is_enabled();
        if (err_code)
            break;
        nrf_delay_ms(10);
    }
}
