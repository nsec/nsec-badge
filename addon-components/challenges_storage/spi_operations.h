#pragma once

void init_ext_spi(void);

void full_duplex_spi_read(uint8_t cmd, unsigned int bytes_amount);
void full_duplex_spi_readR1();
void full_duplex_spi_readR1R2R3();
void full_duplex_spi_writeSRN(uint8_t val, int n);

