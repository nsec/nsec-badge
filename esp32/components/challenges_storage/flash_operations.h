#pragma once

spi_bus_config_t init_spi_bus(void);
esp_flash_t* init_ext_flash(void);

void flash_read(esp_flash_t* _flash, uint16_t size);
void flash_read_at(esp_flash_t* _flash, uint16_t custom_param, unsigned int address);
void flash_write_flag(esp_flash_t* _flash, unsigned int address);
