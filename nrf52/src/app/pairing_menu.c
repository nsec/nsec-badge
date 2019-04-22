//
// Created by nicolas on 4/20/19.
//

#include "pairing_menu.h"
#include "application.h"
#include "gfx_effect.h"
#include "drivers/display.h"

extern uint16_t gfx_width;
extern uint16_t gfx_height;

static const char message[] = "Your passkey is:";

static char passkey[7];

void nsec_ble_show_pairing_menu(const char* key){
    memcpy(passkey, key, 6);
    passkey[6] = '\0';
    application_set(pairing_menu_application);
}

void nsec_ble_hide_pairing_menu(){
    application_clear();
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

void pairing_menu_application(void (*service_callback)()) {
    draw_pairing_menu();

    while (application_get() == pairing_menu_application) {
        service_callback();
    }
}
