/*
 * Copyright 2019 Nicolas Aubry <nicolas.aubry@hotmail.com>
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

#include "pairing_menu.h"
#include "application.h"
#include "gfx_effect.h"
#include "drivers/display.h"
#include "drivers/buttons.h"

#define PASSKEY_SIZE 6

static char passkey[PASSKEY_SIZE + 1];

void nsec_ble_show_pairing_menu(const char* key){
    memcpy(passkey, key, PASSKEY_SIZE);
    passkey[PASSKEY_SIZE] = '\0';
    application_set(pairing_menu_application);
}

void nsec_ble_hide_pairing_menu(void)
{
    if(application_get() == pairing_menu_application){
        application_clear();
    }
}

static void draw_pairing_menu(void)
{
    uint8_t gfx_width = gfx_get_screen_width();
    uint8_t gfx_height = gfx_get_screen_height();

    gfx_fill_rect(0, 0, gfx_width, gfx_height, DISPLAY_BLACK);
    gfx_fill_rect(0, 0, gfx_width, 20, DISPLAY_WHITE);

    gfx_set_text_background_color(DISPLAY_BLACK, DISPLAY_WHITE);

    gfx_set_cursor(28, 6);
    gfx_puts("Bluetooth pairing");

    gfx_set_cursor(32, 30);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_puts("Your passkey is");

    gfx_set_cursor(42, 50);
    gfx_set_text_size(2);

    gfx_puts(passkey);
    gfx_update();

    gfx_set_text_size(1);
}

/**
 * Pressing any button will hide the menu.
 */
static void pairing_menu_handle_buttons(button_t button){
    nsec_ble_hide_pairing_menu();
}

void pairing_menu_application(void (*service_callback)(void))
{
    draw_pairing_menu();

    nsec_controls_add_handler(pairing_menu_handle_buttons);

    while (application_get() == pairing_menu_application) {
        service_callback();
    }
    nsec_controls_suspend_handler(pairing_menu_handle_buttons);
}
