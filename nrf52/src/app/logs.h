#ifndef logs_h
#define logs_h


#include <stdint.h>

#ifdef LOG_CLI

#define NRF_LOG_INFO cli_uart_printf

#else

#include "nrf_log.h"

#endif

void log_error_code(const char*, uint32_t);

void log_init();


#endif // logs_h
