#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "storage_settings.h"

static spi_device_handle_t spi_dev_handle = NULL;

static const char *TAG = "spi_operations";

void init_ext_spi(void) {
    const spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_config, SPI_DMA_CHAN));

    const spi_device_interface_config_t spi_config = {
        .mode = 0,                        // SPI mode (CPOL = 0, CPHA = 0)
        .clock_speed_hz = SPI_MASTER_FREQ_10M,
        .spics_io_num = PIN_CS,           // Chip select GPIO pin number
        .queue_size = 1,                  // Transaction queue size
    };

    ESP_ERROR_CHECK(spi_bus_add_device(HOST_ID, &spi_config, &spi_dev_handle));
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "CRAP with spi_bus_add_device");
    }
}

void full_duplex_spi_read(uint8_t cmd, unsigned int bytes_amount) {
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "Can't execute full_duplex_spi_read, spi_dev_handle is NULL.");
        return;
    }
    /* full duplex */
    uint8_t tx_data[bytes_amount];
    uint8_t rx_data[bytes_amount];
    for (int i = 0; i < bytes_amount; i++) {
        tx_data[i] = 0x00;
        rx_data[i] = 0x00;
    }
    tx_data[0] = cmd;

    spi_transaction_t trans = {
        .length = bytes_amount * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    spi_device_acquire_bus(spi_dev_handle, portMAX_DELAY);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans));
    spi_device_release_bus(spi_dev_handle);
    printf("TX: 0x%02x\nRX: ", tx_data[0]);
    for (int i = 0; i < bytes_amount; i++) {
        printf("0x%02x ", rx_data[i]);
    }
    printf("\n");
}

void full_duplex_spi_readR1() {
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "Can't execute full_duplex_spi_read, spi_dev_handle is NULL.");
        return;
    }
    /* full duplex read of Status Register-1 */
    uint8_t tx_data_r[2] = {0x05, 0x00};
    uint8_t rx_data_r[2] = {0x00, 0x00};
    spi_transaction_t trans_r = {
        .length = 16,
        .tx_buffer = tx_data_r,
        .rx_buffer = rx_data_r
    };
    spi_device_acquire_bus(spi_dev_handle, portMAX_DELAY);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_r));
    spi_device_release_bus(spi_dev_handle);
    printf("TX: 0x%02x\nRX: 0x%02x\n", tx_data_r[0], rx_data_r[1]);
}

void full_duplex_spi_readR1R2R3() {
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "Can't execute full_duplex_spi_read, spi_dev_handle is NULL.");
        return;
    }
    spi_device_acquire_bus(spi_dev_handle, portMAX_DELAY);
    for (int i = 1; i <= 3; i++) {
        /* full duplex read of Status Register-i */
        uint8_t tx_data_r[2];
        if (i == 1) {
            tx_data_r[0] = 0x05;
        } else if (i == 2) {
            tx_data_r[0] = 0x35;
        } else {
            tx_data_r[0] = 0x15;
        }
        tx_data_r[1] = 0x00;
        uint8_t rx_data_r[2] = {0x00, 0x00};
        spi_transaction_t trans_r = {
            .length = 16,
            .tx_buffer = tx_data_r,
            .rx_buffer = rx_data_r
        };
        ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_r));
        printf("Status Register-%d : 0x%02x\n", i, rx_data_r[1]);
    }
    spi_device_release_bus(spi_dev_handle);
}


void full_duplex_spi_writeSRN(uint8_t val, int n) {
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "Can't execute full_duplex_spi_writeSR, spi_dev_handle is NULL.");
        return;
    }

    /* full duplex write */
    unsigned int bytes_amount = 3;
    uint8_t tx_data[bytes_amount];
    uint8_t rx_data[bytes_amount];
    for (int i = 0; i < bytes_amount; i++) {
        tx_data[i] = 0x00;
        rx_data[i] = 0x00;
    }
    
    // Send the Write Enable (WREN) command
    tx_data[0] = 0x06;
    spi_transaction_t trans_wren = {
        .length = 1 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    spi_device_acquire_bus(spi_dev_handle, portMAX_DELAY);
    printf("TX: 0x%02x  RX: 0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_wren));

    // Send the Volatile SR Write Enable command
    tx_data[0] = 0x50;
    spi_transaction_t trans_volsrwe = {
        .length = 1 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    printf("TX: 0x%02x  RX: 0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_volsrwe));

    if (n == 1) {
        tx_data[0] = 0x01;
    } else if (n == 2) {
        tx_data[0] = 0x31;
    } else {
        tx_data[0] = 0x11;
    }

    tx_data[1] = val;
    spi_transaction_t trans = {
        .length = 2 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    printf("TX: 0x%02x  RX: 0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans));
    spi_device_release_bus(spi_dev_handle);

}
