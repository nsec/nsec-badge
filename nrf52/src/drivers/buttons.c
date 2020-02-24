/*
 * Copyright 2018 Eric Tremblay <habscup@gmail.com>
 *           2018 Michael Jeanson <mjeanson@gmail.com>
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

#include "buttons.h"
#include "boards.h"
#include "controls.h"

#include <app_timer.h>

#include <FreeRTOS.h>
#include <queue.h>

/*
 * Queue of button events.
 *
 * The event handler (interrupt context) pushes elements of type `button_t`
 * in the queue.  A task (presumably the UI task) can consume from this queue.
 */
QueueHandle_t button_event_queue = NULL;

/*
 * Delay from a GPIOTE event until a button is reported as pushed (in number of
 * timer ticks).
 */
#define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50)

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void nsec_button_event_handler(uint8_t pin_no, uint8_t button_action) {
    button_t btn;

    if (button_action == APP_BUTTON_PUSH) {
        switch (pin_no) {
        case PIN_INPUT_UP:
            btn = BUTTON_UP;
            break;

        case PIN_INPUT_DOWN:
            btn = BUTTON_DOWN;
            break;

        case PIN_INPUT_BACK:
            btn = BUTTON_BACK;
            break;

        case PIN_INPUT_ENTER:
            btn = BUTTON_ENTER;
            break;

        default:
            /* Should not happen, but just in case. */
            return;
        }
    } else if (button_action == APP_BUTTON_RELEASE) {
        switch (pin_no) {
        case PIN_INPUT_UP:
            btn = BUTTON_UP_RELEASE;
            break;

        case PIN_INPUT_DOWN:
            btn = BUTTON_DOWN_RELEASE;
            break;

        case PIN_INPUT_BACK:
            btn = BUTTON_BACK_RELEASE;
            break;

        case PIN_INPUT_ENTER:
            btn = BUTTON_ENTER_RELEASE;
            break;

        default:
            /* Should not happen, but just in case. */
            return;
        }
    } else {
        /* Should not happen, but just in case. */
        return;
    }

    BaseType_t ret = xQueueSendToBackFromISR(button_event_queue, &btn, NULL);
    APP_ERROR_CHECK_BOOL(ret != errQUEUE_FULL);
    APP_ERROR_CHECK_BOOL(ret == pdPASS);
}

void nsec_buttons_init(void) {
    ret_code_t err_code;

    button_event_queue = xQueueCreate(10, sizeof(button_t));
    APP_ERROR_CHECK_BOOL(button_event_queue != NULL);

    /*
     * The array must be static because a pointer to it will be saved in the
     * button handler module.
     */
    static app_button_cfg_t buttons[] = {
        {PIN_INPUT_UP, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_DOWN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_BACK, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_ENTER, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP,
         nsec_button_event_handler}};

    /*
     * Configure the button library
     */
    err_code = app_button_init(buttons, sizeof(buttons) / sizeof(buttons[0]),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    /*
     * Enable the buttons
     */
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}

bool nsec_button_is_pushed(button_t button) {
    int idx;
    switch (button) {
    case BUTTON_UP:
        idx = 0;
        break;
    case BUTTON_DOWN:
        idx = 1;
        break;
    case BUTTON_BACK:
        idx = 2;
        break;
    case BUTTON_ENTER:
        idx = 3;
        break;
    default:
        return false;
    }

    return app_button_is_pushed(idx);
}
