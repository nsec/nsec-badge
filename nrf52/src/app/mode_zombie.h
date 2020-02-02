//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef mode_zombie_h
#define mode_zombie_h

#define ZOMBIE_TIMER_TIMEOUT			5000 /* ms */
#define ZOMBIE_MODE_DURATION			15000 /* ms */

void mode_zombie_init(void);
void mode_zombie_process(void);
void app_mode_zombie(void (*service_device)(void));

#endif /* mode_zombie_h */
