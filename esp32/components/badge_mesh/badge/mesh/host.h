#include <string.h>

#include <esp_system.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t _device_address_type;
extern uint8_t _device_address[6];
extern bool mesh_host_initialized;

esp_err_t mesh_host_initialize(void);

#ifdef __cplusplus
}
#endif
