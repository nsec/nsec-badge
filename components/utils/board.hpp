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
constexpr unsigned int up = 0;
constexpr unsigned int down = 1;
constexpr unsigned int left = 2;
constexpr unsigned int right = 3;
constexpr unsigned int ok = 4;
constexpr unsigned int cancel = 5;
} // namespace button

/*
 * NeoPixels
 */
namespace neopixel
{
constexpr unsigned int ctrl_pin = 9;
constexpr unsigned int count = 16;
} // namespace neopixel

/*
 * Software serial ports
 */
namespace serial
{
constexpr unsigned int sig_l1 = 2;
constexpr unsigned int sig_l2 = 3;
constexpr unsigned int sig_l3 = 4;
constexpr unsigned int sig_r1 = 5;
constexpr unsigned int sig_r2 = 6;
constexpr unsigned int sig_r3 = 7;
} // namespace serial

} // namespace board
} // namespace nsec
#endif /* NSEC_BOARD_HPP */
