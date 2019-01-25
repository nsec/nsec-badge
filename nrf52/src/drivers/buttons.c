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

#include "boards.h"
#include "buttons.h"
#include "controls.h"

/*
 * Delay from a GPIOTE event until a button is reported as pushed (in number of
 * timer ticks).
 */
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static
void nsec_button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    if (button_action == APP_BUTTON_PUSH) {
        switch (pin_no) {
            case PIN_INPUT_UP:
            	uart_send("+ btn up\n");
                nsec_controls_trigger(BUTTON_UP);
            break;

            case PIN_INPUT_DOWN:
            	uart_send("+ btn down\n");
                nsec_controls_trigger(BUTTON_DOWN);
            break;

            case PIN_INPUT_BACK:
            	uart_send("+ btn back\n");
                nsec_controls_trigger(BUTTON_BACK);
            break;

            case PIN_INPUT_ENTER:
            	uart_send("+ btn enter\n");
                nsec_controls_trigger(BUTTON_ENTER);
            break;
        }
    }
}

void nsec_buttons_init(void) {
    ret_code_t err_code;

    /*
     * The array must be static because a pointer to it will be saved in the
     * button handler module.
     */
    static
    app_button_cfg_t buttons[] =
    {
        {PIN_INPUT_UP,    false, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_DOWN,  false, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_BACK,  false, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler},
        {PIN_INPUT_ENTER, false, NRF_GPIO_PIN_PULLUP, nsec_button_event_handler}
    };

    /*
     * Configure the button library
     */
    err_code = app_button_init(buttons,
            sizeof(buttons) / sizeof(buttons[0]),
                    BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);

    /*
     * Enable the buttons
     */
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}
