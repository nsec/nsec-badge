//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>
#include <stdbool.h>


void battery_init();

// Get the current battery voltage (mV)
// Require a battery_refresh();
uint16_t battery_get_voltage();

void battery_refresh();

bool battery_is_undercharge();

bool battery_is_charging();

bool battery_is_present();

bool battery_is_power_good();

#endif
