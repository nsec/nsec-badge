#include "freertos_run_time_stats.h"

#include "nrfx_rtc.h"

static const nrfx_rtc_t freertos_run_time_stats_rtc = NRFX_RTC_INSTANCE(2);

static void rtc_event_handler(nrfx_rtc_int_type_t int_type)
{
    // Not used.
}

void configure_timer_for_run_time_stats(void)
{
    nrfx_rtc_config_t cfg = NRFX_RTC_DEFAULT_CONFIG;

    // This will give a frequency of 32768 Hz / (1 + 1), so 16384 Hz.
    //
    // The counter is 24 bits wide.  It will wrap after:
    //
    //   (2 ** 24) / 16384 = 1024 seconds
    //
    // Which is about 17 minutes.
    cfg.prescaler = 1;

    nrfx_rtc_init(&freertos_run_time_stats_rtc, &cfg, rtc_event_handler);
    nrfx_rtc_counter_clear(&freertos_run_time_stats_rtc);
    nrfx_rtc_enable(&freertos_run_time_stats_rtc);
}

uint32_t get_run_time_counter_value(void)
{
    return nrfx_rtc_counter_get(&freertos_run_time_stats_rtc);
}
