// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#ifndef NSEC_BOARD_HPP
#define NSEC_BOARD_HPP

#include <driver/uart.h>

namespace nsec::board
{

/*
 * LEDs
namespace led
{
constexpr unsigned int dbg_pin = 13;
} // namespace led
*/
namespace button
{
/*
 * Buttons
 */
constexpr unsigned int up = 0;
constexpr unsigned int down = 1;
//constexpr unsigned int left = 37;
//constexpr unsigned int right = 38;
constexpr unsigned int ok = 9;
//constexpr unsigned int cancel = 40;
} // namespace button

/*
 * NeoPixels
 */
namespace neopixel
{
constexpr unsigned int ctrl_pin = 8;
constexpr unsigned int count = 18;
constexpr unsigned int anim_count = 12;
} // namespace neopixel

/*
 * Software serial ports
 */
namespace serial
{

namespace i2c
{
constexpr unsigned int i2c_num = 0;
constexpr unsigned int i2c_sda = 4;
constexpr unsigned int i2c_scl = 5;
} // namespace i2c

namespace uart
{
constexpr auto uart_device = UART_NUM_1;
constexpr unsigned int tx_pin = 12;
constexpr unsigned int rx_pin = 11;
//constexpr unsigned int signal_pin = 11;
} // namespace uart

/*
namespace right
{
constexpr auto uart_device = UART_NUM_1;
constexpr unsigned int tx_pin = 17;
constexpr unsigned int rx_pin = 18;
constexpr unsigned int signal_pin = 12;
} // namespace right
*/
} // namespace serial

} // namespace nsec::board
#endif /* NSEC_BOARD_HPP */
