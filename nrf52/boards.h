//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef BOARDS_H
#define BOARDS_H

//////////////////////////////////////////////
//Config

// RTC prescaler
#define APP_TIMER_PRESCALER 0

#define UART_BAUDRATE 		UART_BAUDRATE_BAUDRATE_Baud115200

//////////////////////////////////////////////
// PINOUT

// Battery
#define BATT_VOLTAGE    2
#define BATT_CHARGE		27
#define BATT_PGOOD		28

// Display
#define OLED_DATA       4
#define OLED_CLK        3
#define OLED_DC_MODE    9
#define OLED_RESET      7
#define OLED_CS         11

// Button
#define BUTTON_UP		30
#define BUTTON_DOWN		31
#define BUTTON_BACK		6
#define BUTTON_ENTER	5

// UART TO STM32
#define nRF_RXD			13
#define nRF_TXD			12
#define nRF_CTS			14
#define nRF_RTS			15

#define NEOPIXEL 		26

#endif
