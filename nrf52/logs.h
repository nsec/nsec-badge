#ifndef logs_h
#define logs_h


#include <stdint.h>


void log_info(const char* message);

void log_debug(const char* message);

void log_error(const char* message);

void log_warning(const char* message);

void log_error_code(const char*, uint32_t);

void log_init();


#endif // logs_h
