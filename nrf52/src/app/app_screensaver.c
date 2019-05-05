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

#include "app_screensaver.h"
#include "application.h"
#include "drivers/ST7735.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "persistency.h"
#include "timer.h"

#ifdef NSEC_FLAVOR_CONF
#include "nsec_conf_slideshow.h"
#else
#endif

APP_TIMER_DEF(m_screensaver_timer_id);
static bool is_in_screensaver = false;
static uint32_t screensaver_cnt = 0;

/*
 * Callback function
 */
static void screensaver_timer_handler(void *p_context)
{

    /* Only initiate screensaver from the home menu */
    if (application_is_default() && !is_in_screensaver) {
        screensaver_cnt += SCREENSAVER_TIMER_TIMEOUT;

        /* We reached the delay, switch to the sleep app */
        if (screensaver_cnt >= SCREENSAVER_DELAY) {
            screensaver_reset();

            switch (get_stored_screensaver()) {
            case SCREENSAVER_MODE_SLEEP:
                application_set(app_screensaver_sleep);
                break;

            default:
#ifdef NSEC_FLAVOR_CONF
                application_set(nsec_conf_slideshow_app);
#else
#endif
            }
        }
    } else {
        screensaver_reset();
    }
}

static void screensaver_button_handler(button_t button)
{
    /* Whatever the button, return to default app */
    application_clear();
}

/*
 * Initialize the screensaver timer.
 */
void screensaver_init(void)
{
    ret_code_t err_code;

    /* Create the screensaver timer */
    err_code = app_timer_create(&m_screensaver_timer_id, APP_TIMER_MODE_REPEATED,
                                screensaver_timer_handler);
    APP_ERROR_CHECK(err_code);

    /* Start the screensaver timer */
    err_code = app_timer_start(m_screensaver_timer_id,
                               APP_TIMER_TICKS(SCREENSAVER_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);
}

void screensaver_reset(void)
{
    is_in_screensaver = false;
    screensaver_cnt = 0;
}

void app_screensaver_sleep(void (*service_device)())
{
    nsec_controls_add_handler(screensaver_button_handler);

    st7735_display_off();

    is_in_screensaver = true;

    while (application_get() == app_screensaver_sleep) {
        /* Service device and wait for next interrupt */
        service_device();
    }

    /* Exiting sleep mode */
    nsec_controls_clear_handlers();
    is_in_screensaver = false;
    st7735_display_on();
}
