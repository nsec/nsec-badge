//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef timer_h
#define timer_h

#define HEARTBEAT_TIMER_TIMEOUT     50 /* ms */
#define BATTERY_STATUS_TIMER_TIMEOUT 500 /* ms */
#define BATTERY_MANAGER_TIMER_TIMEOUT_MS 1000 /* ms */

void timer_init(void);
uint64_t get_current_time_millis(void);
void start_battery_status_timer(void);
void stop_battery_status_timer(void);
void start_battery_manage_timer(void);

#endif
