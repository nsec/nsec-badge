//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef nsec_nearby_badges_h
#define nsec_nearby_badges_h

#include <stdint.h>

#define NSEC_MAX_NEARBY_BADGES_COUNT (8)

void nsec_nearby_badges_init(void);
void nsec_nearby_badges_callback(uint16_t badge_id, uint8_t addr[], int8_t rssi);
uint8_t nsec_nearby_badges_current_count(void);

#endif /* nsec_nearby_badges_h */
