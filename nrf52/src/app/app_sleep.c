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

#include "app_sleep.h"
#include "application.h"
#include "drivers/ST7735.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "timer.h"

APP_TIMER_DEF(m_sleep_timer_id);
static bool is_sleeping = false;
static uint32_t sleep_cnt = 0;

/*
 * Callback function
 */
static void sleep_timer_handler(void *p_context)
{

    /* Only initiate sleep from the home menu */
    if (application_is_default() && !is_sleeping) {
        sleep_cnt += SLEEP_TIMER_TIMEOUT;

        /* We reached the delay, switch to the sleep app */
        if (sleep_cnt >= SLEEP_DELAY) {
            sleep_reset();
            application_set(app_sleep);
        }
    } else {
        sleep_reset();
    }
}

static void sleep_button_handler(button_t button)
{
    /* Whatever the button, return to default app */
    application_clear();
}

/*
 * Initialize the sleep timer
 */
void sleep_init(void)
{
    ret_code_t err_code;

    /* Create the sleep timer */
    err_code = app_timer_create(&m_sleep_timer_id, APP_TIMER_MODE_REPEATED,
                                sleep_timer_handler);
    APP_ERROR_CHECK(err_code);

    /* Start the sleep timer */
    err_code = app_timer_start(m_sleep_timer_id,
                               APP_TIMER_TICKS(SLEEP_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);
}

void sleep_reset(void)
{
    sleep_cnt = 0;
}

void app_sleep(void (*service_device)())
{
    nsec_controls_add_handler(sleep_button_handler);

    st7735_display_off();

    is_sleeping = true;

    while (application_get() == app_sleep) {
        /* Service device and wait for next interrupt */
        service_device();
    }

    /* Exiting sleep mode */
    nsec_controls_clear_handlers();
    is_sleeping = false;
    st7735_display_on();
}
