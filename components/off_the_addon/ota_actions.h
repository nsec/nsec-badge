#pragma once
#include <string>

bool write_flash_to_ota(esp_flash_t* _flash);

void storage_read_from_ota(int ota, esp_flash_t* _flash);

int ota_cmd(int argc, char **argv);

std::string get_firmware_select_string();
