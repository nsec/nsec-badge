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

#define UART_BAUDRATE         UART_BAUDRATE_BAUDRATE_Baud115200

#if defined(BOARD_SPUTNIK) || defined(BOARD_SPUTNIK_PROTO)
//////////////////////////////////////////////
// PINOUT

// Battery
#define PIN_BATT_VOLTAGE        2
#define PIN_BATT_CHARGE         27
#define PIN_BATT_PGOOD          28

// Display
#define PIN_OLED_DATA           4
#define PIN_OLED_CLK            3
#define PIN_OLED_DC_MODE        9
#define PIN_OLED_RESET          7
#define PIN_OLED_CS             11

// Button
#define PIN_INPUT_UP            30
#define PIN_INPUT_DOWN          31
#define PIN_INPUT_BACK          6
#define PIN_INPUT_ENTER         5

// UART TO STM32
#define PIN_nRF_RXD             13
#define PIN_nRF_TXD             12
#define PIN_nRF_CTS             14
#define PIN_nRF_RTS             15

#define PIN_NEOPIXEL            26


#else // BOARD_BRAIN

// TODO: Update PIN when the brain will be used for development
//       for now the development is done on the sputnik board
// LED
#define PIN_LED_STATUS_1        0
#define PIN_LED_STATUS_2        1
#define PIN_NEOPIXEL            25

// Battery
#define PIN_BATT_VOLTAGE        2
// Brain does not have pin for PGOOD and CHG

// SPI
#define PIN_MOSI                9
#define PIN_MISO                8
#define PIN_SCK                 7

// TWI
#define PIN_TWI_SDA             27
#define PIN_TW_SCL              26

// Flash SPI
#define PIN_FLASH_MOSI          3
#define PIN_FLASH_MISO          5
#define PIN_FLASH_CLK           4
#define PIN_FLASH_CS            6

// Display
#define PIN_OLED_DATA           PIN_MOSI
#define PIN_OLED_CLK            PIN_SCK
#define PIN_OLED_DC_MODE        11
#define PIN_OLED_RESET          10
#define PIN_OLED_CS             12
#define PIN_OLED_BLK            13

// Button
#define PIN_INPUT_UP            30
#define PIN_INPUT_DOWN          28
#define PIN_INPUT_BACK          29
#define PIN_INPUT_ENTER         31

// UART TO STM32
#define PIN_nRF_RXD             22 // STM32 TXD
#define PIN_nRF_TXD             20 // STM32 RXD
#define PIN_nRF_CTS             23 // STM32 RTS
#define PIN_nRF_RTS             24 // STM32 CTS

#endif // BOARD_SPUTNIK | BOARD_SPUTNIK_PROTO
#endif // BOARDS_H
