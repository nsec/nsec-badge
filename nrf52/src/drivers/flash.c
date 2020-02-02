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
#include <nrfx_spim.h>

#include "flash.h"

#include "boards.h"

#define FLASH_WRITE_COMMAND 0x2
#define FLASH_READ_COMMAND 0x3
#define READ_STATUS_REGISTER_1_COMMAND 0x5
#define WRITE_ENABLE_COMMAND 0x6
#define FLASH_ERASE_4K_COMMAND 0x20

#define READ_STATUS_REGISTER_1_BUSY 0x1

static const nrfx_spim_t m_spi_master_0 = NRFX_SPIM_INSTANCE(CONF_FLASH_SPI_INST);


/* Initialize the external flash module.  */

void flash_init(void)
{
    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;

    config.frequency = NRF_SPIM_FREQ_8M;
    config.mosi_pin = PIN_FLASH_MOSI;
    config.miso_pin = PIN_FLASH_MISO;
    config.sck_pin = PIN_FLASH_CLK;
    config.ss_pin = PIN_FLASH_CS;

    APP_ERROR_CHECK(nrfx_spim_init(&m_spi_master_0, &config, NULL, NULL));
}

static ret_code_t flash_read_status_register_1(uint8_t *value) {
    uint8_t tx_data = READ_STATUS_REGISTER_1_COMMAND;
    uint8_t rx_data[2];

    nrfx_spim_xfer_desc_t xfer_desc = {&tx_data, sizeof(tx_data), rx_data, sizeof(rx_data)};

    ret_code_t ret = nrfx_spim_xfer(&m_spi_master_0, &xfer_desc, 0);
    if (ret != NRF_SUCCESS)
        return ret;

    *value = rx_data[1];

    return NRF_SUCCESS;
}

/* Wait until the "busy" bit is cleared.  */

static ret_code_t flash_wait_for_completion(void)
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

ret_code_t flash_read_128(int address, uint8_t *data) {
    uint8_t tx_data[4];
    uint8_t rx_data[sizeof(tx_data) + 128];

    tx_data[0] = FLASH_READ_COMMAND;
    tx_data[1] = (address >> 16) & 0xff;
    tx_data[2] = (address >> 8) & 0xff;
    tx_data[3] = address & 0xff;

    nrfx_spim_xfer_desc_t xfer_desc = {tx_data, sizeof(tx_data), rx_data, sizeof(rx_data)};

    ret_code_t ret =
        nrfx_spim_xfer(&m_spi_master_0, &xfer_desc, 0);
    if (ret != NRF_SUCCESS) {
        return ret;
    }

    memcpy(data, rx_data + sizeof(tx_data), 128);

    return NRF_SUCCESS;
}

static ret_code_t write_enable(void)
{
    uint8_t tx_data = WRITE_ENABLE_COMMAND;
    nrfx_spim_xfer_desc_t xfer_desc = {&tx_data, sizeof(tx_data), NULL, 0};

    return nrfx_spim_xfer(&m_spi_master_0, &xfer_desc, 0);
}

/* Erase the 4096-bytes block of data containing ADDRESS.  */

ret_code_t flash_erase(int address) {
    ret_code_t ret = write_enable();
    if (ret != NRF_SUCCESS)
        return ret;

    uint8_t tx_data[4];
    tx_data[0] = FLASH_ERASE_4K_COMMAND;
    tx_data[1] = (address >> 16) & 0xff;
    tx_data[2] = (address >> 8) & 0xff;
    tx_data[3] = address & 0xff;

    nrfx_spim_xfer_desc_t xfer_desc = {tx_data, sizeof(tx_data), NULL, 0};

    ret = nrfx_spim_xfer(&m_spi_master_0, &xfer_desc, 0);
    if (ret != NRF_SUCCESS)
        return ret;

    return flash_wait_for_completion();
}

/* Write 128 bytes of flash.  The address must be a multiple of 128.  */

ret_code_t flash_write_128(int address, const uint8_t *data) {
    if (address % 128 != 0) {
        return NRF_ERROR_INVALID_PARAM;
    }

    ret_code_t ret = write_enable();
    if (ret != NRF_SUCCESS)
        return ret;

    uint8_t tx_data[1 + 3 + 128];
    tx_data[0] = FLASH_WRITE_COMMAND;
    tx_data[1] = (address >> 16) & 0xff;
    tx_data[2] = (address >> 8) & 0xff;
    tx_data[3] = address & 0xff;
    memcpy(tx_data + 4, data, 128);

    nrfx_spim_xfer_desc_t xfer_desc = {tx_data, sizeof(tx_data), NULL, 0};

    ret = nrfx_spim_xfer(&m_spi_master_0, &xfer_desc, 0);
    if (ret != NRF_SUCCESS)
        return ret;

    return flash_wait_for_completion();
}
