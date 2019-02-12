//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef GUI_H
#define GUI_H

/* Status bar */
#define STATUS_BAR_POS_X		128
#define STATUS_BAR_POS_Y		0
#define STATUS_BAR_POS          STATUS_BAR_POS_X, STATUS_BAR_POS_Y

#define STATUS_BAR_BG_COLOR		0x4B1A
#define STATUS_BAR_WIDTH		32
#define STATUS_BAR_HEIGHT		22

#define BATTERY_POS_X			143
#define BATTERY_POS_Y			6
#define BATTERY_POS 			BATTERY_POS_X, BATTERY_POS_Y

#define BLE_LOGO_POS_X			129
#define BLE_LOGO_POS_Y			2
#define BLE_LOGO_POS 			BLE_LOGO_POS_X, BLE_LOGO_POS_Y

/* Home menu */
#define HOME_MENU_BG_COLOR		0x4AD8
#define HOME_MENU_WIDTH			32
#define HOME_MENU_HEIGHT		80 - STATUS_BAR_HEIGHT


#endif // GUI_H
