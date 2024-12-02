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
 */
namespace led
{
constexpr unsigned int dbg_pin = 13;
} // namespace led

namespace button
{
/*
 * Buttons
 */
constexpr unsigned int up = 35;
constexpr unsigned int down = 36;
constexpr unsigned int left = 37;
constexpr unsigned int right = 38;
constexpr unsigned int ok = 39;
constexpr unsigned int cancel = 40;
} // namespace button

/*
 * NeoPixels
 */
namespace neopixel
{
constexpr unsigned int ctrl_pin = 48;
constexpr unsigned int count = 16;
} // namespace neopixel

/*
 * Software serial ports
 */
namespace serial
{
namespace dev
{
constexpr unsigned int tx_pin = 43;
constexpr unsigned int rx_pin = 44;
} // namespace dev

namespace left
{
constexpr auto uart_device = UART_NUM_1;
constexpr unsigned int tx_pin = 15;
constexpr unsigned int rx_pin = 16;
constexpr unsigned int signal_pin = 11;
} // namespace left

namespace right
{
constexpr auto uart_device = UART_NUM_1;
constexpr unsigned int tx_pin = 17;
constexpr unsigned int rx_pin = 18;
constexpr unsigned int signal_pin = 12;
} // namespace right

} // namespace serial

} // namespace nsec::board
#endif /* NSEC_BOARD_HPP */
