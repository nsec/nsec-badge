#include <stdint.h>
#include <stdbool.h>

#include <libopencm3/stm32/tsc.h>

#include "tsc_n.h"

//
// Helper functions
//

// Discharge all IOs, block.
void tsc_discharge_io(void) {
    tsc_set_io_mode_pushpull_low();
}

//
// TSC functions
//

// Control register
void tsc_set_high_charge_pulse_transfer(uint32_t high) {
    TSC_CR |= (high & TSC_CR_CTPH_MASK) << TSC_CR_CTPH_SHIFT;
}

void tsc_set_low_charge_pulse_transfer(uint32_t low) {
    TSC_CR |= (low & TSC_CR_CTPL_MASK) << TSC_CR_CTPL_SHIFT;
}

void tsc_enable_spread_spectrum(void) {
    TSC_CR |= TSC_CR_SSE;
}

void tsc_set_spread_spectrum_deviation(uint32_t deviation) {
    TSC_CR |= (deviation & TSC_CR_SSD_MASK) << TSC_CR_SSD_SHIFT;
}

void tsc_set_spread_spectrum_prescaler(uint32_t prescaler) {
    TSC_CR |= (prescaler & TSC_CR_SSP_MASK) << TSC_CR_SSP_SHIFT;
}

void tsc_set_pulse_generator_prescaler(uint32_t prescaler) {
    TSC_CR |= (prescaler & TSC_CR_PGPSC_MASK) << TSC_CR_PGPSC_SHIFT;
}

void tsc_set_max_count_value(uint32_t value) {
    TSC_CR |= (value & TSC_CR_MCV_MASK) << TSC_CR_MCV_SHIFT;
}

void tsc_set_io_mode_pushpull_low(void) {
    TSC_CR &= ~TSC_CR_IODEF;
}

void tsc_set_io_mode_floating(void) {
    TSC_CR |= TSC_CR_IODEF;
}

void tsc_set_sync_polarization_rising_edge(void) {
    TSC_CR |= TSC_CR_SYNCPOL;
}

void tsc_set_sync_acquisition_mode(void) {
    TSC_CR |= TSC_CR_AM;
}

void tsc_start_acquisition(void) {
    TSC_CR |= TSC_CR_START;
}

void tsc_enable(void) {
    TSC_CR |= TSC_CR_TSCE;
}

// Interrupt enable register
void tsc_enable_max_count_error_interrupt(void) {
    TSC_IER |= TSC_IER_MCEIE;
}

bool tsc_get_max_count_error_flag(void) {
    return TSC_ISR & TSC_ISR_MCEF;
}

void tsc_clear_max_count_error_flag(void) {
    TSC_ICR |= TSC_ICR_MCEIC;
}

void tsc_enable_end_of_acquisition_interrupt(void) {
    TSC_IER |= TSC_IER_EOAIE;
}

bool tsc_get_end_of_acquisition_flag(void) {
    return TSC_ISR & TSC_ISR_EOAF;
}

void tsc_clear_end_of_acquisition_flag(void) {
    TSC_ICR |= TSC_ICR_EOAIC;
}

// Hysteris control register
void tsc_clear_schmitt_trigger_hyst(uint32_t index) {
    TSC_IOHCR &= ~TSC_IOBIT_VAL(index, 1);
    TSC_IOHCR &= ~TSC_IOBIT_VAL(index, 2);
    TSC_IOHCR &= ~TSC_IOBIT_VAL(index, 3);
    TSC_IOHCR &= ~TSC_IOBIT_VAL(index, 4);
}

// Enable the acquisition (counter is counting) on the corresponding analog I/O
// group |index|.
void tsc_enable_group(uint32_t index) {
    TSC_IOGCSR |= TSC_IOGCSR_GxE(index);
}

// Disable the acquisition (counter is counting) on the corresponding analog
// I/O group |index|.
void tsc_disable_group(uint32_t index) {
    TSC_IOGCSR &= ~TSC_IOGCSR_GxE(index);
}

bool tsc_is_group_enabled(uint32_t index) {
    return TSC_IOGCSR & TSC_IOGCSR_GxE(index);
}

// Check whether the acquisition is ongoing or completed on group |index|.
bool tsc_is_group_acquisition_completed(uint32_t index) {
    return TSC_IOGCSR & TSC_IOGCSR_GxS(index);
}

// Return the number of charge transfer cycles generated on the analog I/O
// group |index| to complete its acquisition.
uint32_t tsc_group_read_counter(uint32_t index) {
    //return TSC_IOGxCR(index) & 0x3fff;
    return TSC_IOGxCR(index);
}

// Enable the |io| on group |index| as a channel I/O.
void tsc_enable_io_channel(uint32_t index, uint32_t io) {
    TSC_IOCCR |= TSC_IOBIT_VAL(index, io);
}

// Clear all io on group |index|.
void tsc_clear_io_channel(uint32_t index) {
    TSC_IOCCR &= ~TSC_IOBIT_VAL(index, 1);
    TSC_IOCCR &= ~TSC_IOBIT_VAL(index, 2);
    TSC_IOCCR &= ~TSC_IOBIT_VAL(index, 3);
    TSC_IOCCR &= ~TSC_IOBIT_VAL(index, 4);
}

// Enable the sampling of |io| on group |index|.
void tsc_enable_io_sampling(uint32_t index, uint32_t io) {
    TSC_IOSCR |= TSC_IOBIT_VAL(index, io);
}

// Clear all sampling io on group |index|.
void tsc_clear_io_sampling(uint32_t index) {
    TSC_IOSCR &= ~TSC_IOBIT_VAL(index, 1);
    TSC_IOSCR &= ~TSC_IOBIT_VAL(index, 2);
    TSC_IOSCR &= ~TSC_IOBIT_VAL(index, 3);
    TSC_IOSCR &= ~TSC_IOBIT_VAL(index, 4);
}
