//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef timer_h
#define timer_h

#define HEARTBEAT_TIMER_TIMEOUT     50 /* ms */

void timer_init(void);
uint64_t get_current_time_millis(void);

#endif
