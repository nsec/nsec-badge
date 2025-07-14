// SPDX-FileCopyrightText: 2025 NorthSec
//
// SPDX-License-Identifier: MIT

#ifndef NSEC_BOARD_HPP
#define NSEC_BOARD_HPP

#include "soc/gpio_num.h"
#include <driver/uart.h>

namespace nsec::board
{

namespace button
{
/*
 * Buttons
 */
constexpr unsigned int up = 0;
constexpr unsigned int down = 1;
constexpr unsigned int ok = 9;
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
constexpr unsigned int i2c_slave_addr = 0x68;
} // namespace i2c

namespace uart
{
constexpr auto uart_device = UART_NUM_1;
constexpr unsigned int tx_pin = 12;
constexpr unsigned int rx_pin = 11;
} // namespace uart

} // namespace serial

namespace ir
{
constexpr gpio_num_t tx_pin = static_cast<gpio_num_t>(10);
constexpr gpio_num_t rx_pin = static_cast<gpio_num_t>(3);
} // namespace ir


namespace dock
{
constexpr unsigned int input1_pin = 2;
constexpr unsigned int input2_pin = 6;
constexpr unsigned int output_pin = 7;
}

} // namespace nsec::board
#endif /* NSEC_BOARD_HPP */
