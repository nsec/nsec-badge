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
#include "nsec_led_settings_color.h"
#include "persistency.h"

#include "FreeRTOS.h"
#include "queue.h"

static uint8_t g_color_idx;

static void set_value(int value)
{
    setArrayColor_packed_WS2812FX(value, g_color_idx);
    update_stored_color(0, value, g_color_idx, true);
}

static const nsec_led_settings_base_element elements[] = {
    {
        .label = "Red",
        .value = RED,
    },
    {
        .label = "Green",
        .value = GREEN,
    },
    {
        .label = "Blue",
        .value = BLUE,
    },
    {
        .label = "White",
        .value = WHITE,
    },
    {
        .label = "Black",
        .value = BLACK,
    },
    {
        .label = "Yellow",
        .value = YELLOW,
    },
    {
        .label = "Cyan",
        .value = CYAN,
    },
    {
        .label = "Magenta",
        .value = MAGENTA,
    },
    {
        .label = "Purple",
        .value = PURPLE,
    },
    {
        .label = "Orange",
        .value = ORANGE,
    },
};

void nsec_show_led_settings_color(uint8_t color_idx)
{
    g_color_idx = color_idx;

    int initial_value = getArrayColor_WS2812FX(color_idx);
    nsec_show_led_settings_base(elements, ARRAY_SIZE(elements), initial_value,
                                set_value);
}
