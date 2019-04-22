//
// Created by nicolas on 4/20/19.
//

#ifndef NRF52_PAIRING_MENU_H
#define NRF52_PAIRING_MENU_H

#include <peer_manager.h>

void show_pairing_menu(const char* passkey);
void display_message(const char* message);
void display_peer_params(ble_gap_sec_params_t* peer_params);

#endif //NRF52_PAIRING_MENU_H
