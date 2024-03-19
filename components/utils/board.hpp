// SPDX-FileCopyrightText: 2024 NorthSec
//
// SPDX-License-Identifier: MIT

#ifndef NSEC_BOARD_HPP
#define NSEC_BOARD_HPP

namespace nsec
{
namespace board
{

/*
 * LEDs
 */
namespace led
{
constexpr unsigned int dbg = 13;
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
constexpr unsigned int txd = 43;
constexpr unsigned int rxd = 44;
} // namespace dev

namespace right
{
constexpr unsigned int txd = 17;
constexpr unsigned int rxd = 18;
constexpr unsigned int signal = 12;
} // namespace right

namespace left
{
constexpr unsigned int txd = 15;
constexpr unsigned int rxd = 16;
constexpr unsigned int signal = 11;
} // namespace left

} // namespace serial

} // namespace board
} // namespace nsec
#endif /* NSEC_BOARD_HPP */
