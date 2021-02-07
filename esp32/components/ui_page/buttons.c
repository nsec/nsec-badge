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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "buttons.h"

/*
 * Queue of button events.
 *
 * The event handler (interrupt context) pushes elements of type `button_t`
 * in the queue.  A task (presumably the UI task) can consume from this queue.
 */
QueueHandle_t button_event_queue = NULL;

static void nsec_button_event_task(void *arg)
{
    button_t button;
    int level;

    uint16_t buttons_history[6] = {};
    uint8_t buttons_pins[] = {PIN_INPUT_BACK, PIN_INPUT_DOWN,  PIN_INPUT_ENTER,
                              PIN_INPUT_LEFT, PIN_INPUT_RIGHT, PIN_INPUT_UP};

    while (true) {
        for (int i = 0; i < 6; ++i) {
            level = gpio_get_level(buttons_pins[i]);

            buttons_history[i] = (buttons_history[i] << 1) | level;
            switch (buttons_history[i] & 0b1111000000011111) {
            case 0b0000000000011111:
                buttons_history[i] = 0xffff;
                level = 1;
                break;

            case 0b1111000000000000:
                buttons_history[i] = 0;
                level = 0;
                break;

            default:
                continue;
            }

            switch (buttons_pins[i]) {
            case PIN_INPUT_BACK:
                button = level ? BUTTON_BACK : BUTTON_BACK_RELEASE;
                break;

            case PIN_INPUT_DOWN:
                button = level ? BUTTON_DOWN : BUTTON_DOWN_RELEASE;
                break;

            case PIN_INPUT_ENTER:
                button = level ? BUTTON_ENTER : BUTTON_ENTER_RELEASE;
                break;

            case PIN_INPUT_LEFT:
                button = level ? BUTTON_LEFT : BUTTON_LEFT_RELEASE;
                break;

            case PIN_INPUT_RIGHT:
                button = level ? BUTTON_RIGHT : BUTTON_RIGHT_RELEASE;
                break;

            case PIN_INPUT_UP:
                button = level ? BUTTON_UP : BUTTON_UP_RELEASE;
                break;

            default:
                continue;
            }

            BaseType_t ret = xQueueSendToBack(button_event_queue, &button, 0);
            if (ret != errQUEUE_FULL) {
                assert(ret == pdPASS && "Buttons queue push failed.");
            }
        }

        vTaskDelay(1);
    }
}

void nsec_buttons_init(void)
{
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

    xTaskCreate(nsec_button_event_task, "nsec_button_event_task", 1024, NULL,
                10, NULL);
}

bool nsec_button_is_pushed(button_t button)
{
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
