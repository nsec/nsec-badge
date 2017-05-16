//
//  nsec_nearby_badges.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-05-15.
//
//

#ifndef nsec_nearby_badges_h
#define nsec_nearby_badges_h

#include <stdint.h>

#define NSEC_MAX_NEARBY_BADGES_COUNT (8)

void nsec_nearby_badges_init(void);
void nsec_nearby_badges_callback(uint16_t badge_id, uint8_t addr[], int8_t rssi);
uint8_t nsec_nearby_badges_current_count(void);

#endif /* nsec_nearby_badges_h */
