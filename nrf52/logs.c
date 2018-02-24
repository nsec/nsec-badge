#include "logs.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


void log_error_code(const char* function_name, uint32_t error_code){
	NRF_LOG_INFO("%s: %s", function_name, nrf_strerror_get(error_code));
}

void log_info(const char* message){
	nrf_log_frontend_std_0(3, message);
}

void log_debug(const char* message){
	nrf_log_frontend_std_0(4, message);
}

void log_warning(const char* message){
	nrf_log_frontend_std_0(2, message);
}

void log_error(const char* message){
	nrf_log_frontend_std_0(1, message);
}

void log_init(){
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
