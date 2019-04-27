//
// Created by nicolas on 4/20/19.
//

#include "pairing_menu.h"
#include "application.h"
#include "gfx_effect.h"
#include "drivers/display.h"
#include "drivers/buttons.h"

#define PASSKEY_SIZE 6

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static const char message[] = "Your passkey is:";

static char passkey[PASSKEY_SIZE + 1];

void nsec_ble_show_pairing_menu(const char* key){
    memcpy(passkey, key, PASSKEY_SIZE);
    passkey[PASSKEY_SIZE] = '\0';
    application_set(pairing_menu_application);
}

void nsec_ble_hide_pairing_menu(){
    if(application_get() == pairing_menu_application){
        application_clear();
    }
}

static void draw_pairing_menu(){
    gfx_fill_rect(0, 0, gfx_width, gfx_height, DISPLAY_BLACK);
    gfx_set_cursor(0, 0);
    gfx_set_text_size(1);
    gfx_set_text_color(DISPLAY_WHITE);
    gfx_puts(message);
    gfx_set_cursor(gfx_width/2 - 20, 20);
    gfx_set_text_size(2);
    gfx_puts(passkey);
    gfx_update();
}

/**
 * Pressing any button will hide the menu.
 */
static void pairing_menu_handle_buttons(button_t button){
    nsec_ble_hide_pairing_menu();
}

void pairing_menu_application(void (*service_callback)()) {
    draw_pairing_menu();

    nsec_controls_add_handler(pairing_menu_handle_buttons);

    while (application_get() == pairing_menu_application) {
        service_callback();
    }
    nsec_controls_suspend_handler(pairing_menu_handle_buttons);
}
