#pragma once

void challenge_storage1_code(int delay);
void storage_read_from_ota(int ota, esp_flash_t* _flash);
void storage_write_to_ota(int ota, esp_flash_t* _flash);
void erase_ota(int ota);
