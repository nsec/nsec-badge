/*
 * Copyright 2017 Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 *           2017 Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
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

#ifndef BOARDS_H
#define BOARDS_H

#if defined(BOARD_PCA10040)

// TODO: Add PCA10040 configuration

#elif defined(BOARD_SPUTNIK)
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
#define CONF_OLED_SPI_INST       2
#define CONF_OLED_PWM_INST       2

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


#elif defined(BOARD_BRAIN)

// TODO: Update PIN when the brain will be used for development
//       for now the development is done on the sputnik board
// LED
#define PIN_LED_STATUS_1        0
#define PIN_LED_STATUS_2        1
#define PIN_NEOPIXEL            25

// Battery
#define PIN_BATT_VOLTAGE        2
// Brain does not have pin for PGOOD and CHG

// TWI
#define PIN_TWI_SDA             27
#define PIN_TW_SCL              26

// Flash SPI
#define PIN_FLASH_MOSI          3
#define PIN_FLASH_MISO          5
#define PIN_FLASH_CLK           4
#define PIN_FLASH_CS            6

// Display
#define PIN_OLED_DATA           9
#define PIN_OLED_CLK            7
#define PIN_OLED_DC_MODE        11
#define PIN_OLED_RESET          10
#define PIN_OLED_CS             12
#define PIN_OLED_BLK            13
#define CONF_OLED_SPI_INST       2
#define CONF_OLED_PWM_INST       2

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

#else
#error "Unknown board."
#endif

#endif // BOARDS_H
