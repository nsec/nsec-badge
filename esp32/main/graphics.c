#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string.h>

#include "esp_log.h"
#include "st7789.h"

#include "graphics.h"

/* In-memory framebuffer with the contents of the screen. */
static pixel_t *display_buffer = NULL;

/* Device state object for the display driver. */
static TFT_t display_device = {};

/* Bitfield index of updated rows that need to be flushed. */
static uint8_t display_rows_hot[DISPLAY_WIDTH / 8] = {};

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

// Display functions.

/**
 * Put a single RGB bitmap pixel into the framebuffer.
 */
static inline void graphics_put_pixel(uint8_t x, uint8_t y, uint8_t r,
                                      uint8_t g, uint8_t b)
{
    // Treat all very dark colors as faux transparent and skip these pixels.
    // Cannot simply compare to #000000 because decompressed JPEG will instead
    // have some approximate color near black.
    if (r < 18 && g < 18 && b < 18)
        return;

    // Store colors in RGB565 to use 2 bytes per pixel, instead of 3 bytes.
    pixel_t new_pixel = rgb565_conv(r, g, b);

    // Swap the two bytes of the color to achieve the memory layout that can be
    // piped directly to the display without additional processing.
    new_pixel = ((new_pixel >> 8) & 0xFF) + ((new_pixel << 8) & 0xFF00);

    // Skip unchanged pixels to allow optimized partial screen updates.
    pixel_t pixel = display_buffer[x * DISPLAY_HEIGHT + y];
    if (new_pixel == pixel)
        return;

    display_buffer[x * DISPLAY_HEIGHT + y] = new_pixel;

    uint8_t index_byte = x / 8;
    uint8_t index_bit = x % 8;
    display_rows_hot[index_byte] |= 1 << index_bit;
}

/**
 * Allocate pixel buffers for display data.
 */
static void graphics_display_buffers_allocate()
{
    assert(display_buffer == NULL);

    display_buffer = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(pixel_t));
    assert(display_buffer != NULL);
    ESP_LOGI(__FUNCTION__, "display_buffer is at %p", display_buffer);
}

/**
 * Send the framebuffer to the display.
 *
 * Calculate the portion of the rows that need to be updated on the screen and
 * send all of them in a single SPI transaction, excluding unchanged pixels to
 * the left and to the right of this region. This approach tries to find a
 * balance between the cost of setting up an SPI transaction, copying pixels
 * into a temporary buffer and sending extra unchanged portions of the screen.
 */
void graphics_update_display()
{
    uint8_t address[] = {0, 0, 0, 0};
    uint8_t index_bit = 0;
    uint8_t index_byte = 0;
    uint8_t slice_end = 0;
    uint8_t slice_start = DISPLAY_WIDTH;
    uint8_t x = 0;

    for (; index_byte < DISPLAY_WIDTH / 8; index_byte++, x += 8) {
        for (index_bit = 0; index_bit < 8; index_bit++) {
            if (0 == (display_rows_hot[index_byte] & (1 << index_bit)))
                continue;

            display_rows_hot[index_byte] ^= (1 << index_bit);

            if (slice_start > x + index_bit)
                slice_start = x + index_bit;

            slice_end = x + index_bit + 1;
        }
    }

    if (slice_end < slice_start)
        return;

    address[1] = 0;
    address[3] = 240;
    display_spi_write(0x2A, address, 4);

    address[1] = slice_start;
    address[3] = slice_end;
    display_spi_write(0x2B, address, 4);

    display_spi_write(
        0x2C, (uint8_t *)(display_buffer + (slice_start * DISPLAY_HEIGHT)),
        ((slice_end - slice_start) * DISPLAY_HEIGHT * 2));
}

// Badge initialization functions.

/**
 * Initialize devices and memory buffers.
 */
void graphics_start()
{
    ESP_LOGI(__FUNCTION__, "Starting graphics system. Free heap is %d.",
             esp_get_free_heap_size());

    graphics_display_buffers_allocate();

    display_spi_bus_init();
    lcdInit(&display_device, DISPLAY_PHY_WIDTH, DISPLAY_PHY_HEIGHT, 0, 0);
    lcdFillScreen(&display_device, BLUE);
    gpio_set_level(DISPLAY_SPI_BL, 1);

    ESP_LOGI(__FUNCTION__, "Graphics system initialized. Free heap is %d.",
             esp_get_free_heap_size());
}
