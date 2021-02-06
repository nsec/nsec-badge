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

#include "driver/gpio.h"

#include "buttons.h"

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
#define BUTTON_DETECTION_DELAY 50

/**@brief Function for handling events from the button handler module.
 */
static void nsec_button_event_handler(void *arg) {
    button_t btn;
    uint32_t pin_no = (uint32_t) arg;

    if (gpio_get_level(pin_no)) {
        switch (pin_no) {
        case PIN_INPUT_BACK:
            btn = BUTTON_BACK;
            break;

        case PIN_INPUT_DOWN:
            btn = BUTTON_DOWN;
            break;

        case PIN_INPUT_ENTER:
            btn = BUTTON_ENTER;
            break;

        case PIN_INPUT_LEFT:
            btn = BUTTON_LEFT;
            break;

        case PIN_INPUT_RIGHT:
            btn = BUTTON_RIGHT;
            break;

        case PIN_INPUT_UP:
            btn = BUTTON_UP;
            break;

        default:
            /* Should not happen, but just in case. */
            return;
        }
    } else {
        switch (pin_no) {
        case PIN_INPUT_BACK:
            btn = BUTTON_BACK_RELEASE;
            break;

        case PIN_INPUT_DOWN:
            btn = BUTTON_DOWN_RELEASE;
            break;

        case PIN_INPUT_ENTER:
            btn = BUTTON_ENTER_RELEASE;
            break;

        case PIN_INPUT_LEFT:
            btn = BUTTON_LEFT_RELEASE;
            break;

        case PIN_INPUT_RIGHT:
            btn = BUTTON_RIGHT_RELEASE;
            break;

        case PIN_INPUT_UP:
            btn = BUTTON_UP_RELEASE;
            break;

        default:
            /* Should not happen, but just in case. */
            return;
        }
    }

    BaseType_t ret = xQueueSendToBackFromISR(button_event_queue, &btn, NULL);
    assert(ret != errQUEUE_FULL && "Buttons queue is full.");
    assert(ret == pdPASS && "Buttons queue push failed.");
}

void nsec_buttons_init(void) {
    button_event_queue = xQueueCreate(10, sizeof(button_t));
    assert(button_event_queue != NULL && "Failed to create buttons queue.");

    unsigned long long bit_mask = 0;
    bit_mask |= 1ULL << PIN_INPUT_BACK;
    bit_mask |= 1ULL << PIN_INPUT_DOWN;
    bit_mask |= 1ULL << PIN_INPUT_ENTER;
    bit_mask |= 1ULL << PIN_INPUT_LEFT;
    bit_mask |= 1ULL << PIN_INPUT_RIGHT;
    bit_mask |= 1ULL << PIN_INPUT_UP;


    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = bit_mask;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(PIN_INPUT_BACK, nsec_button_event_handler, (void *)PIN_INPUT_BACK);
    gpio_isr_handler_add(PIN_INPUT_DOWN, nsec_button_event_handler, (void *)PIN_INPUT_DOWN);
    gpio_isr_handler_add(PIN_INPUT_ENTER, nsec_button_event_handler, (void *)PIN_INPUT_ENTER);
    gpio_isr_handler_add(PIN_INPUT_LEFT, nsec_button_event_handler, (void *)PIN_INPUT_LEFT);
    gpio_isr_handler_add(PIN_INPUT_RIGHT, nsec_button_event_handler, (void *)PIN_INPUT_RIGHT);
    gpio_isr_handler_add(PIN_INPUT_UP, nsec_button_event_handler, (void *)PIN_INPUT_UP);
}

bool nsec_button_is_pushed(button_t button) {
    int idx;
    switch (button) {
    case BUTTON_BACK:
        idx = PIN_INPUT_BACK;
        break;
    case BUTTON_DOWN:
        idx = PIN_INPUT_DOWN;
        break;
    case BUTTON_ENTER:
        idx = PIN_INPUT_ENTER;
        break;
    case BUTTON_LEFT:
        idx = PIN_INPUT_LEFT;
        break;
    case BUTTON_RIGHT:
        idx = PIN_INPUT_RIGHT;
        break;
    case BUTTON_UP:
        idx = PIN_INPUT_UP;
        break;
    default:
        return false;
    }

    return gpio_get_level(idx) == 1;
}
