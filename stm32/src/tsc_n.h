#ifndef TSC_N_H
#define TSC_N_H

#include <stdint.h>
#include <stdbool.h>

void tsc_discharge_io(void);
bool tsc_group_get_status(uint32_t);
uint32_t tsc_group_get_value(uint32_t);
void tsc_set_high_charge_pulse_transfer(uint32_t);
void tsc_set_low_charge_pulse_transfer(uint32_t low);
void tsc_enable_spread_spectrum(void);
void tsc_set_spread_spectrum_deviation(uint32_t deviation);
void tsc_set_spread_spectrum_prescaler(uint32_t prescaler);
void tsc_set_pulse_generator_prescaler(uint32_t prescaler);
void tsc_set_max_count_value(uint32_t value);
void tsc_set_io_mode_pushpull_low(void);
void tsc_set_io_mode_floating(void);
void tsc_set_sync_polarization_rising_edge(void);
void tsc_set_sync_acquisition_mode(void);
void tsc_start_acquisition(void);
void tsc_enable(void);
void tsc_enable_max_count_error_interrupt(void);
bool tsc_get_max_count_error_flag(void);
void tsc_clear_max_count_error_flag(void);
void tsc_enable_end_of_acquisition_interrupt(void);
bool tsc_get_end_of_acquisition_flag(void);
void tsc_clear_end_of_acquisition_flag(void);
void tsc_enable_group(uint32_t index);
void tsc_disable_group(uint32_t index);
bool tsc_is_group_enabled(uint32_t index);
bool tsc_is_group_acquisition_completed(uint32_t index);
uint32_t tsc_group_read_counter(uint32_t index);
void tsc_enable_io_channel(uint32_t index, uint32_t io);
void tsc_clear_io_channel(uint32_t index);
void tsc_enable_io_sampling(uint32_t index, uint32_t io);
void tsc_clear_io_sampling(uint32_t index);

void tsc_clear_schmitt_trigger_hyst(uint32_t);

#endif


