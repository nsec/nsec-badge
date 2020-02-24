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
#include <string.h>

#include "nsec_warning.h"

#include "text_box.h"
#include "main_menu.h"
#include "home_menu.h"
#include "gui.h"
#include "drivers/display.h"
#include "drivers/controls.h"
#include "gfx_effect.h"

static void warning_handle_buttons(button_t button);
const char *warning_notice = "That device is equipped with a protected lithium-ion battery. However, it is still a lithium-ion battery and it should be manipulated with caution. Do not put the battery under excessive heat or direct sunlight. Do not reverse the battery polarity to avoid damaging your badge. Do not use an unprotected lithium-ion battery into the badge, the badge does not have an under voltage protection. The actual battery in the badge will automatically shutdown when reaching 2.5V. An unprotected battery will drain until 0V and it can be dangerous to recharge afterwards. Do not use the battery to power a device that required AA battery. Those batteries are the same size as the one in the badge but the voltage of an AA battery is 1.5V and the voltage of this battery is between 4.2V and 2.5V depending on the charge. You could fry your beautiful TV remote and we don't want that.";

static struct text_box_config config = {
    GEN_MENU_POS_X,
    22,
    160,
    GEN_MENU_HEIGHT,
    HOME_MENU_BG_COLOR,
    DISPLAY_WHITE
};

static void draw_warning_title(void)
{
    draw_title("WARNING", 25, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

void nsec_warning_show(void) {
    nsec_controls_add_handler(warning_handle_buttons);
    gfx_fill_rect(0, 0, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_warning_title();
    text_box_init(warning_notice, &config);
}

static void warning_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        redraw_home_menu_burger_selected();
        nsec_controls_suspend_handler(warning_handle_buttons);
    }
}
