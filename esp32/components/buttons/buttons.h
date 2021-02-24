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

#ifndef buttons_h
#define buttons_h

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define PIN_INPUT_BACK 35
#define PIN_INPUT_DOWN 4
#define PIN_INPUT_ENTER 34
#define PIN_INPUT_LEFT 17
#define PIN_INPUT_RIGHT 16
#define PIN_INPUT_UP 5

typedef enum {
    BUTTON_NONE,
    BUTTON_BACK,
    BUTTON_BACK_RELEASE,
    BUTTON_DOWN,
    BUTTON_DOWN_RELEASE,
    BUTTON_ENTER,
    BUTTON_ENTER_RELEASE,
    BUTTON_LEFT,
    BUTTON_LEFT_RELEASE,
    BUTTON_RIGHT,
    BUTTON_RIGHT_RELEASE,
    BUTTON_UP,
    BUTTON_UP_RELEASE,
} button_t;

extern QueueHandle_t button_event_queue;

void nsec_buttons_flush(void);
void nsec_buttons_init(void);
bool nsec_button_is_pushed(button_t button);

#endif
