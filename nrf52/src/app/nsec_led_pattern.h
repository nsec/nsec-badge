//  Copyright (c) 2018
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef nsec_led_pattern_h
#define nsec_led_pattern_h

void nsec_led_pattern_show(void);
int8_t get_extra_array_index(uint8_t mode);

bool pattern_is_unlock(uint32_t sponsor_index);

#endif /* nsec_led_pattern_h */
