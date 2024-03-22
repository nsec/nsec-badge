#pragma once

void flash_read(esp_flash_t* _flash, uint16_t size);
void flash_read_at(esp_flash_t* _flash, uint16_t custom_param, unsigned int address);

