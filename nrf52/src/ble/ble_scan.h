//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef NRF52_OBSERVER_H
#define NRF52_OBSERVER_H

#include <stdbool.h>
#include <stdint.h>


void configure_scan(bool active_scan, uint16_t timeout, uint16_t interval, uint16_t window);

void start_scan();

void stop_scan();

#endif //NRF52_OBSERVER_H
