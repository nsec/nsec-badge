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

#include <nordic_common.h>
#include <nrf.h>
#include <stdbool.h>
#include <string.h>

#include "drivers/buttons.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "menu.h"
#include "nsec_led_settings.h"
#include "nsec_led_settings_base.h"
#include "nsec_led_settings_speed.h"
#include "persistency.h"

#include "FreeRTOS.h"
#include "queue.h"

static void set_value(int value)
{
    setSpeed_WS2812FX(value);
    update_stored_speed(0, getSpeed_WS2812FX(), true);
}

static const nsec_led_settings_base_element elements[] = {
    {
        .label = "Super slow",
        .value = SUPER_SLOW_SPEED,
    },
    {
        .label = "Slow",
        .value = SLOW_SPEED,
    },
    {
        .label = "Medium",
        .value = MEDIUM_SPEED,
    },
    {
        .label = "Fast",
        .value = FAST_SPEED,
    },
    {
        .label = "Super fast",
        .value = SUPER_FAST_SPEED,
    },
};

void nsec_show_led_settings_speed(void)
{
    int initial_value = getSpeed_WS2812FX();
    nsec_show_led_settings_base(elements, ARRAY_SIZE(elements), initial_value,
                                set_value);
}
