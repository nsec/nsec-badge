#ifndef SRC_APP_FREERTOS_RUN_TIME_STATS
#define SRC_APP_FREERTOS_RUN_TIME_STATS

#include <stdint.h>

void configure_timer_for_run_time_stats(void);
uint32_t get_run_time_counter_value(void);

#endif
