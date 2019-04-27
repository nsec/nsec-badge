//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef GUI_H
#define GUI_H

/*

+------------------------+--------+
|                        | STATUS |
|                        | BAR    |
+------------------------+--------+
|                        |        |
| HOME_MENU              | BURGER |
|                        | MENU   |
|                        |        |
+------------------------+--------+

*/

/* TEXT */
#define TEXT_BASE_HEIGHT        8
#define TEXT_BASE_WIDTH         6

/* Status bar */
#define STATUS_BAR_POS_X        128
#define STATUS_BAR_POS_Y        0
#define STATUS_BAR_POS          STATUS_BAR_POS_X, STATUS_BAR_POS_Y

#define STATUS_BAR_BG_COLOR     0x4B1A
#define STATUS_BAR_WIDTH        32
#define STATUS_BAR_HEIGHT       22

#define BATTERY_POS_X           141
#define BATTERY_POS_Y           6
#define BATTERY_POS             BATTERY_POS_X, BATTERY_POS_Y

#define BLE_LOGO_POS_X          131
#define BLE_LOGO_POS_Y          5
#define BLE_LOGO_POS            BLE_LOGO_POS_X, BLE_LOGO_POS_Y

/* Home menu */
#define HOME_MENU_BG_COLOR      0x4AD7
#define HOME_MENU_POS_X         128
#define HOME_MENU_POS_Y         STATUS_BAR_HEIGHT
#define HOME_MENU_POS           HOME_MENU_POS_X, HOME_MENU_POS_Y
#define HOME_MENU_WIDTH         32
#define HOME_MENU_HEIGHT        (80 - STATUS_BAR_HEIGHT)

#define BURGER_MENU_POS_X       138
#define BURGER_MENU_POS_Y       30
#define BURGER_MENU_POS         BURGER_MENU_POS_X, BURGER_MENU_POS_Y
#define BURGER_MENU_CUR_Y       32
#define BURGER_MENU_TITLE_X     45

#define SETTINGS_MENU_POS_X     138
#define SETTINGS_MENU_POS_Y     55
#define SETTINGS_MENU_POS       SETTINGS_MENU_POS_X, SETTINGS_MENU_POS_Y
#define SETTINGS_MENU_CUR_Y     58
#define SETTINGS_MENU_TITLE_X   20

#define NEUROSOFT_LOGO_POS      30, 10
#define NSEC_LOGO_POS           15, 15
#define CONF_STR_POS            35, 60

/* General Menu */
#define GEN_MENU_POS_X          1
#define GEN_MENU_POS_Y          20
#define GEN_MENU_POS            GEN_MENU_POS_X, GEN_MENU_POS_Y
#define GEN_MENU_WIDTH          127
#define GEN_MENU_HEIGHT         60

/* Conference text box */
#define CONF_POS_X              1
#define CONF_POS_Y              22
#define CONF_POS                CONF_POS_X, CONF_POS_Y
#define CONF_WIDTH              160
#define CONF_HEIGHT             58

/* Led Settings */
#define LED_SET_VAL_POS_X       1
#define LED_SET_VAL_POS_Y       20
#define LED_SET_VAL_POS         LED_SET_VAL_POS_X, LED_SET_VAL_POS_Y
#define LED_SET_POS_X           1
#define LED_SET_POS_Y           32
#define LED_SET_POS             LED_SET_POS_X, LED_SET_POS_Y
#define LED_SET_WIDTH           128
#define LED_SET_HEIGHT          48

/* Input Box */
#define INPUT_BOX_POS_X         12
#define INPUT_BOX_POS_Y         32
#define INPUT_BOX_BRA_WIDTH     5

#endif // GUI_H
