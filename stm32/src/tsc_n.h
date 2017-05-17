#ifndef TSC_N_H
#define TSC_N_H

#include <stdint.h>
#include <stdbool.h>

#define TSC_GROUP_INDEX_1   1
#define TSC_GROUP_INDEX_2   2
#define TSC_GROUP_INDEX_3   3
#define TSC_GROUP_INDEX_4   4

#define TSC_CTP_1   0
#define TSC_CTP_2   1
#define TSC_CTP_3   2
#define TSC_CTP_4   3
#define TSC_CTP_5   4
#define TSC_CTP_6   5
#define TSC_CTP_7   6
#define TSC_CTP_8   7
#define TSC_CTP_9   8
#define TSC_CTP_10  9
#define TSC_CTP_11  10
#define TSC_CTP_12  11
#define TSC_CTP_13  12
#define TSC_CTP_14  13
#define TSC_CTP_15  14
#define TSC_CTP_16  15

#define TSC_CR_CTPH_MASK 0xf
#define TSC_CR_CTPL_MASK 0xf

#define TSC_CR_SSD_MASK 0x7f

#define TSC_CR_SSP_SHIFT    0xf
#define TSC_CR_SSP_MASK     0x1

#define TSC_SSPSC_0     0
#define TSC_SSPSC_2     1

#define TSC_CR_PGPSC_MASK     0x7

#define TSC_PGPSC_0     0
#define TSC_PGPSC_2     1
#define TSC_PGPSC_4     2
#define TSC_PGPSC_8     3
#define TSC_PGPSC_16    4
#define TSC_PGPSC_32    5
#define TSC_PGPSC_64    6
#define TSC_PGPSC_128   7

#define TSC_CR_MCV_MASK     0x7

#define TSC_MCV_255     0
#define TSC_MCV_511     1
#define TSC_MCV_1023    2
#define TSC_MCV_2047    3
#define TSC_MCV_4095    4
#define TSC_MCV_8191    5
#define TSC_MCV_16383   6

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


