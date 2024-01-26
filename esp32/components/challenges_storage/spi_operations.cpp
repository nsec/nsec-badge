#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "esp_log.h"

static spi_device_handle_t spi_dev_handle = NULL;

static const char *TAG = "spi_operations";

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
    printf("TX cmd: 0x%02x\nrx: ", tx_data[0]);
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
    printf("TX cmd: 0x%02x\nrx: 0x%02x\n", tx_data_r[0], rx_data_r[1]);
}


void full_duplex_spi_writeSR1(uint8_t val) {
    if (spi_dev_handle == NULL) {
        ESP_LOGI(TAG, "Can't execute full_duplex_spi_writeSR, spi_dev_handle is NULL.");
        return;
    }

    /* full duplex read of Status Register-2 */
    uint8_t tx_data_r[2] = {0x35, 0x00};
    uint8_t rx_data_r[2] = {0x00, 0x00};
    spi_transaction_t trans_r = {
        .length = 16,
        .tx_buffer = tx_data_r,
        .rx_buffer = rx_data_r
    };
    spi_device_acquire_bus(spi_dev_handle, portMAX_DELAY);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_r));
    spi_device_release_bus(spi_dev_handle);
printf("I see that Register 2 is at %02x\n", rx_data_r[1]);
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
    printf("TX cmd: 0x%02x  val:0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_wren));

    // Send the Volatile SR Write Enable command
    tx_data[0] = 0x50;
    spi_transaction_t trans_volsrwe = {
        .length = 1 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    printf("TX cmd: 0x%02x  val:0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans_volsrwe));

    tx_data[0] = 0x01;
    tx_data[1] = val;
    tx_data[2] = 0x40; //rx_data_r[1]; // send the same value to Register 2
    spi_transaction_t trans = {
        .length = 3 * 8,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    printf("TX cmd: 0x%02x  val:0x%02x\n", tx_data[0], tx_data[1]);
    ESP_ERROR_CHECK(spi_device_transmit(spi_dev_handle, &trans));
    spi_device_release_bus(spi_dev_handle);

}
