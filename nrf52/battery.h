//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>
#include <stdbool.h>

// Let the device use the battery to power itself
void battery_connect();

// Disconnect the battery
void battery_disconnect();

// Get the current battery voltage (mV)
// Require a battery_refresh();
uint16_t battery_get_voltage();

// Initialize the battery module
void battery_init();

// Check if the battery is undercharge
bool battery_is_undercharge();

// Refresh the battery voltage
void battery_refresh();

bool battery_is_charging();

bool battery_is_present();

void nsec_battery_manager_init(void);

#endif
