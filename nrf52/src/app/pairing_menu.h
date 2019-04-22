//
// Created by nicolas on 4/20/19.
//

#ifndef NRF52_PAIRING_MENU_H
#define NRF52_PAIRING_MENU_H

#include <peer_manager.h>

void nsec_ble_show_pairing_menu(const char* passkey);
void pairing_menu_application(void (*service_callback)());
void nsec_ble_hide_pairing_menu();

#endif //NRF52_PAIRING_MENU_H
