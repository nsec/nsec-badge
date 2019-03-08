/*
 * Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>

#include <app_util_platform.h>
#include <nrf_drv_spi.h>

#include "boards.h"

#define SPI_DEFAULT_CONFIG_IRQ_PRIORITY APP_IRQ_PRIORITY_LOW

#define FLASH_WRITE_COMMAND 0x2
#define FLASH_READ_COMMAND 0x3
#define READ_STATUS_REGISTER_1_COMMAND 0x5
#define WRITE_ENABLE_COMMAND 0x6
#define FLASH_ERASE_4K_COMMAND 0x20

#define READ_STATUS_REGISTER_1_BUSY 0x1

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

/* Initialize the external flash module.  */

void flash_init() {
  nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;

  config.frequency = NRF_DRV_SPI_FREQ_8M;
  config.mosi_pin = PIN_FLASH_MOSI;
  config.miso_pin = PIN_FLASH_MISO;
  config.sck_pin = PIN_FLASH_CLK;
  config.ss_pin = PIN_FLASH_CS;

  APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi_master_0, &config, NULL, NULL));
}

static ret_code_t flash_read_status_register_1(uint8_t *value)
{
    uint8_t tx = READ_STATUS_REGISTER_1_COMMAND;
    uint8_t rx[2];

    ret_code_t ret = nrf_drv_spi_transfer(&m_spi_master_0, &tx, 1, rx, 2);
    if (ret != NRF_SUCCESS)
        return ret;

    *value = rx[1];

    return NRF_SUCCESS;
}

/* Wait until the "busy" bit is cleared.  */

static ret_code_t flash_wait_for_completion()
{
    while (1) {
        uint8_t status;
        ret_code_t ret = flash_read_status_register_1(&status);
        if (ret != NRF_SUCCESS)
            return ret;

        if ((status & READ_STATUS_REGISTER_1_BUSY) == 0)
            return NRF_SUCCESS;
    }
}

/* Read 128 bytes of flash.  */

ret_code_t flash_read_128 (int address, uint8_t *data) {
  uint8_t tx[4];
  tx[0] = FLASH_READ_COMMAND;
  tx[1] = (address >> 16) & 0xff;
  tx[2] = (address >> 8) & 0xff;
  tx[3] = address & 0xff;

  uint8_t rx[sizeof(tx) + 128];
  ret_code_t ret =
      nrf_drv_spi_transfer(&m_spi_master_0, tx, sizeof(tx), rx, sizeof(rx));
  if (ret != NRF_SUCCESS) {
      return ret;
  }

  memcpy(data, rx + sizeof(tx), 128);

  return NRF_SUCCESS;
}

static ret_code_t write_enable() {
  uint8_t tx = WRITE_ENABLE_COMMAND;

  return nrf_drv_spi_transfer(&m_spi_master_0, &tx, 1, NULL, 0);
}

/* Erase the 4096-bytes block of data containing ADDRESS.  */

ret_code_t flash_erase(int address) {
    ret_code_t ret = write_enable();
    if (ret != NRF_SUCCESS) return ret;

    uint8_t tx[4];
    tx[0] = FLASH_ERASE_4K_COMMAND;
    tx[1] = (address >> 16) & 0xff;
    tx[2] = (address >> 8) & 0xff;
    tx[3] = address & 0xff;
    ret = nrf_drv_spi_transfer(&m_spi_master_0, tx, sizeof(tx), NULL, 0);
    if (ret != NRF_SUCCESS) return ret;

    return flash_wait_for_completion();
}

/* Write 128 bytes of flash.  The address must be a multiple of 128.  */

ret_code_t flash_write_128(int address, const uint8_t *data) {
  if (address % 128 != 0) {
    return NRF_ERROR_INVALID_PARAM;
  }

  ret_code_t ret = write_enable();
  if (ret != NRF_SUCCESS) return ret;

  uint8_t tx[1 + 3 + 128];
  tx[0] = FLASH_WRITE_COMMAND;
  tx[1] = (address >> 16) & 0xff;
  tx[2] = (address >> 8) & 0xff;
  tx[3] = address & 0xff;
  memcpy(tx + 4, data, 128);

  ret = nrf_drv_spi_transfer(&m_spi_master_0, tx, sizeof(tx), NULL, 0);
  if (ret != NRF_SUCCESS) return ret;

  return flash_wait_for_completion();
}
