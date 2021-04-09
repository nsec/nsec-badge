/*
 Arduino.h - Main include file for the Arduino SDK
 Copyright (c) 2005-2013 Arduino Team.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef HAL_ESP32_HAL_H_
#define HAL_ESP32_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "sdkconfig.h"
#include "esp_system.h"

#ifndef F_CPU
#define F_CPU (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000U)
#endif

//forward declaration from freertos/portmacro.h
void vPortYield(void);
void yield(void);
#define optimistic_yield(u)

#define ESP_REG(addr) *((volatile uint32_t *)(addr))
#define NOP() asm volatile ("nop")

// BB: see how many we can remove

#include "esp32-hal-log.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal-cpu.h"

#ifndef BOARD_HAS_PSRAM
#ifdef CONFIG_SPIRAM_SUPPORT
#undef CONFIG_SPIRAM_SUPPORT
#endif
#endif

// BB a few key functions from esp32-hal-misc, which are defined
// bar Arduino and thus therea re no headers for here
//
unsigned long micros(void);
unsigned long millis(void);

//returns chip temperature in Celsius
float temperatureRead(void);


unsigned long micros();
unsigned long millis();
void delay(uint32_t);
void delayMicroseconds(uint32_t us);

#if !CONFIG_ESP32_PHY_AUTO_INIT
void arduino_phy_init();
#endif


#ifdef __cplusplus
}
#endif

#endif /* HAL_ESP32_HAL_H_ */
