#pragma once
#include <string>

bool write_flash_to_ota(esp_flash_t* _flash);

#if CTF_ADDON_ADMIN_MODE
void storage_read_from_ota(int ota, esp_flash_t* _flash);
void erase_ota(int ota);
#endif

int ota_cmd(int argc, char **argv);

std::string get_firmware_select_string();
