#ifndef logs_h
#define logs_h


#include <stdint.h>
#include "nrf_log.h"

void log_error_code(const char*, uint32_t);

void log_init();


#endif // logs_h
