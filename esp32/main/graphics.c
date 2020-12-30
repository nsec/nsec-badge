#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string.h>

#include "esp_log.h"
#include "st7789.h"

#include "graphics.h"

/* Device state object for the display driver. */
static TFT_t display_device = {};

/* SPI handler of the display device. */
static spi_device_handle_t display_spi_handler;

// SPI bus functions.

/**
 * Initialize the display device on the SPI bus.
 *
 * Use custom initialization instead using the driver library function to
 * set the correct bus configuration.
 */
static void display_spi_bus_init()
{
    esp_err_t ret;

    gpio_pad_select_gpio(DISPLAY_SPI_BL);
    gpio_set_direction(DISPLAY_SPI_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(DISPLAY_SPI_BL, 0);

    gpio_pad_select_gpio(DISPLAY_SPI_DC);
    gpio_set_direction(DISPLAY_SPI_DC, GPIO_MODE_OUTPUT);
    gpio_set_level(DISPLAY_SPI_DC, 0);

    gpio_pad_select_gpio(DISPLAY_SPI_RST);
    gpio_set_direction(DISPLAY_SPI_RST, GPIO_MODE_OUTPUT);
    for (uint8_t i = 1; i < 4; i++) {
        gpio_set_level(DISPLAY_SPI_RST, i % 2);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // Set max_transfer_sz to be bigger than the display framebuffer size.
    spi_bus_config_t bus_config = {
        .max_transfer_sz = 131071,
        .miso_io_num = -1,
        .mosi_io_num = DISPLAY_SPI_MOSI,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .sclk_io_num = DISPLAY_SPI_CLK,
    };

    ret = spi_bus_initialize(HSPI_HOST, &bus_config, 2);
    assert(ret == ESP_OK);

    spi_device_interface_config_t device_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_40M,
        .queue_size = 7,
        .mode = 2,
        .flags = SPI_DEVICE_NO_DUMMY,
        .spics_io_num = -1,
    };

    ret = spi_bus_add_device(HSPI_HOST, &device_config, &display_spi_handler);
    assert(ret == ESP_OK);

    // Set all fields expected by the st7789 component.
    display_device._dc = DISPLAY_SPI_DC;
    display_device._bl = DISPLAY_SPI_BL;
    display_device._SPIHandle = display_spi_handler;
}

/**
 * Write directly into display SPI without using st7789 driver API.
 *
 * Send data directly from a memory address without copying it into a temporary
 * buffer, which is not efficient.
 */
static void display_spi_write(uint8_t command, uint8_t *data, uint32_t length)
{
    spi_transaction_t transaction;

    gpio_set_level(DISPLAY_SPI_DC, 0);
    memset(&transaction, 0, sizeof(spi_transaction_t));

    transaction.flags |= SPI_TRANS_USE_TXDATA;
    transaction.length = 8;
    transaction.tx_data[0] = command;
    spi_device_transmit(display_spi_handler, &transaction);

    if (length > 0) {
        gpio_set_level(DISPLAY_SPI_DC, 1);
        memset(&transaction, 0, sizeof(spi_transaction_t));

        transaction.length = length * 8;
        transaction.tx_buffer = data;
        spi_device_transmit(display_spi_handler, &transaction);
    }
}

// Badge initialization functions.

/**
 * Initialize devices and memory buffers.
 */
void graphics_start()
{
    ESP_LOGI(__FUNCTION__, "Starting graphics system. Free heap is %d.",
             esp_get_free_heap_size());

    display_spi_bus_init();
    lcdInit(&display_device, DISPLAY_PHY_WIDTH, DISPLAY_PHY_HEIGHT, 0, 0);
    lcdFillScreen(&display_device, BLUE);
    gpio_set_level(DISPLAY_SPI_BL, 1);

    ESP_LOGI(__FUNCTION__, "Graphics system initialized. Free heap is %d.",
             esp_get_free_heap_size());
}
