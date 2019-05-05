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

#include <app_timer.h>
#include <nrf_delay.h>

#include "app_flashlight.h"
#include "application.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gfx_effect.h"
#include "nsec_led_settings.h"
#include "persistency.h"


static void flashlight_button_handler(button_t button)
{
    /* Whatever the button, return to default app */
    application_clear();
}

void app_flashlight(void (*service_device)())
{
    gfx_fill_screen(DISPLAY_WHITE);
    setMode_WS2812FX(FX_MODE_STATIC);
    setBrightness_WS2812FX(255);
    setColor_packed_WS2812FX(WHITE);
    service_WS2812FX();

    // Gros hask, je comprends pas
    nsec_controls_disable();
    nrf_delay_ms(1000);
    nsec_controls_enable();

    nsec_controls_add_handler(flashlight_button_handler);

    while (application_get() == app_flashlight) {
        /* Service device and wait for next interrupt */
        service_device();
    }

    /* Exiting flashlight mode */
    nsec_controls_clear_handlers();

    load_led_settings();
    display_set_brightness(get_stored_display_brightness());
}
