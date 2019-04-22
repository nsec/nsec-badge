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

void display_message(const char* _message){
    gfx_fill_rect(0, 0, gfx_width, gfx_height, DISPLAY_BLACK);
    gfx_set_cursor(0, 0);
    gfx_set_text_size(1);
    gfx_set_text_color(DISPLAY_WHITE);
    gfx_puts(_message);
    gfx_update();
}

void display_peer_params(ble_gap_sec_params_t* peer_params){
    char bond[] = "bond: %d";
    char mitm[] = "mitm: %d";
    char lesc[] = "lesc: %d";
    char keypress[] = "keypress: %d";
    char iocaps[] = "iocaps: %d";
    char oob[] = "oob: %d";
    char min_key_size[] = "min key size: %d";
    char max_key_size[] = "max_key_size: %d";
    char buffer[30];

    gfx_fill_rect(0, 0, gfx_width, gfx_height, DISPLAY_BLACK);
    gfx_set_text_size(1);
    gfx_set_text_color(DISPLAY_WHITE);

    gfx_set_cursor(0, 0);
    snprintf(buffer, sizeof(buffer), bond, peer_params->bond);
    gfx_puts(buffer);

    gfx_set_cursor(0, 10);
    snprintf(buffer, sizeof(buffer), mitm, peer_params->mitm);
    gfx_puts(buffer);

    gfx_set_cursor(0, 20);
    snprintf(buffer, sizeof(buffer), lesc, peer_params->lesc);
    gfx_puts(buffer);

    gfx_set_cursor(0, 30);
    snprintf(buffer, sizeof(buffer), keypress, peer_params->keypress);
    gfx_puts(buffer);

    gfx_set_cursor(0, 40);
    snprintf(buffer, sizeof(buffer), iocaps, peer_params->io_caps);
    gfx_puts(buffer);

    gfx_set_cursor(0, 50);
    snprintf(buffer, sizeof(buffer), oob, peer_params->oob);
    gfx_puts(buffer);

    gfx_set_cursor(0, 60);
    snprintf(buffer, sizeof(buffer), min_key_size, peer_params->min_key_size);
    gfx_puts(buffer);

    gfx_set_cursor(0, 70);
    snprintf(buffer, sizeof(buffer), max_key_size, peer_params->max_key_size);
    gfx_puts(buffer);

    gfx_update();
}

void pairing_menu_application(void (*service_callback)()) {
    draw_pairing_menu();

    while (application_get() == pairing_menu_application) {
        service_callback();
    }
}
