#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string.h>

#include "esp32/rom/tjpgd.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "freertos/task.h"
#include "st7789.h"

#include "graphics.h"

#define TJPGD_WORK_SZ 3100

typedef struct {
    FILE *fp;
    uint8_t offset_x;
    uint8_t offset_y;
} jpeg_session_device;

/* In-memory framebuffer with the contents of the screen. */
static pixel_t *display_buffer = NULL;

/* Device state object for the display driver. */
static TFT_t display_device = {};

/* Bitfield index of updated rows that need to be flushed. */
static uint8_t display_rows_hot[DISPLAY_WIDTH / 8] = {};

/* SPI handler of the display device. */
static spi_device_handle_t display_spi_handler;

/* TJpgDec work area. */
static char *tjpgd_work = NULL;

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

// Sprite collection functions.

/**
 * Initialize storage filesystem.
 */
static void graphics_collection_start()
{
    esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                  .partition_label = NULL,
                                  .max_files = 8,
                                  .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        abort();
    }
}

// Display functions.

/**
 * Put a single high color (RGB565) pixel into the framebuffer.
 *
 * The hight and low bytes of the pixel value must already be swapped. This is
 * an optimisation used to achieve the same memory layout that is used by the
 * screen.
 */
static inline void graphics_put_pixel(uint8_t x, uint8_t y, pixel_t new_pixel)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
        return;

    // Zero pixel is a special value for transparency.
    if (new_pixel == 0)
        return;

    // Store columns in memory in the reverse order to achieve the correct
    // mirrored appearance of the image on the display that is mounted
    // sideways.
    uint8_t rx = DISPLAY_WIDTH - x - 1;

    // Skip unchanged pixels to allow optimized partial screen updates.
    pixel_t pixel = display_buffer[rx * DISPLAY_HEIGHT + y];
    if (new_pixel == pixel)
        return;

    display_buffer[rx * DISPLAY_HEIGHT + y] = new_pixel;

    uint8_t index_byte = rx / 8;
    uint8_t index_bit = rx % 8;
    display_rows_hot[index_byte] |= 1 << index_bit;
}

/**
 * Put a single RGB bitmap pixel into the framebuffer.
 */
static inline void graphics_put_pixel_rgb(uint8_t x, uint8_t y, uint8_t r,
                                          uint8_t g, uint8_t b)
{
    // Treat all very dark colors as faux transparent and skip these pixels.
    // Cannot simply compare to #000000 because decompressed JPEG will instead
    // have some approximate color near black.
    if (r < 20 && g < 20 && b < 20)
        return;

    // Store colors in RGB565 to use 2 bytes per pixel, instead of 3 bytes.
    pixel_t new_pixel = rgb565_conv(r, g, b);

    // Swap the two bytes of the color to achieve the memory layout that can be
    // piped directly to the display without additional processing.
    new_pixel = ((new_pixel >> 8) & 0xFF) + ((new_pixel << 8) & 0xFF00);

    graphics_put_pixel(x, y, new_pixel);
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

    tjpgd_work = calloc(TJPGD_WORK_SZ, sizeof(char));
    assert(tjpgd_work != NULL);
}

/**
 * Decoder input callback for TJpgDec.
 *
 * Implements http://elm-chan.org/fsw/tjpgd/en/input.html.
 */
static UINT graphics_jpeg_decode_infunc(JDEC *decoder, BYTE *buffer, UINT ndata)
{
    jpeg_session_device *session_device =
        (jpeg_session_device *)decoder->device;

    if (buffer == NULL) {
        fseek(session_device->fp, ndata, SEEK_CUR);
        return ndata;
    }

    return fread(buffer, 1, ndata, session_device->fp);
}

/**
 * Decoder output callback for TJpgDec.
 *
 * Implements http://elm-chan.org/fsw/tjpgd/en/output.html.
 */
static UINT graphics_jpeg_decode_outfunc(JDEC *decoder, void *bitmap,
                                         JRECT *rect)
{
    uint8_t *rgb = (uint8_t *)bitmap;
    jpeg_session_device *session_device =
        (jpeg_session_device *)decoder->device;

    for (int y = rect->top; y <= rect->bottom; y++) {
        uint16_t offset_y = session_device->offset_y + y;
        if (offset_y >= DISPLAY_HEIGHT)
            break;

        for (int x = rect->left; x <= rect->right; x++, rgb += 3) {
            uint16_t offset_x = session_device->offset_x + x;
            if (offset_x >= DISPLAY_WIDTH)
                continue;

            graphics_put_pixel_rgb(offset_x, offset_y, rgb[0], rgb[1], rgb[2]);
        }
    }

    return 1;
}

/**
 * Draw a single JPEG image sprite into the display buffer.
 */
void graphics_draw_jpeg(const char *name, uint8_t x, uint8_t y)
{
    int result;

    jpeg_session_device session_device = {
        .offset_x = x,
        .offset_y = y,
    };

    session_device.fp = fopen(name, "rb");
    if (session_device.fp == NULL) {
        ESP_LOGW(__FUNCTION__, "Cannot open sprite image \"%s\"", name);
        return;
    }

    JDEC decoder;
    result = jd_prepare(&decoder, graphics_jpeg_decode_infunc, tjpgd_work,
                        TJPGD_WORK_SZ, (void *)&session_device);
    if (result != JDR_OK) {
        ESP_LOGE(__FUNCTION__, "jd_prepare failed for image \"%s\"", name);
        goto out;
    }

    result = jd_decomp(&decoder, graphics_jpeg_decode_outfunc, 0);
    if (result != JDR_OK) {
        ESP_LOGE(__FUNCTION__, "jd_decomp failed for image \"%s\"", name);
        goto out;
    }

out:
    fclose(session_device.fp);
}

/**
 * Draw a single JPEG tile into the display buffer.
 *
 * This is a simple wrapper around graphics_draw_jpeg() that allows to use
 * coordinates on the tile grid.
 */
void graphics_draw_jpeg_tile(const char *name, uint8_t tile_x, uint8_t tile_y)
{
    uint8_t x = tile_x * DISPLAY_TILE_WIDTH;
    uint8_t y = tile_y * DISPLAY_TILE_HEIGHT;
    return graphics_draw_jpeg(name, x, y);
}

/**
 * Draw a single sprite image from the library into the display buffer.
 */
void graphics_draw_sprite(uint8_t index, uint8_t x, uint8_t y)
{
    // FIXME
}

/**
 * Draw a single sprite tile from the library into the display buffer.
 *
 * This is a simple wrapper around graphics_draw_sprite() that allows to use
 * coordinates on the tile grid.
 */
void graphics_draw_sprite_tile(uint8_t index, uint8_t tile_x, uint8_t tile_y)
{
    uint8_t x = tile_x * DISPLAY_TILE_WIDTH;
    uint8_t y = tile_y * DISPLAY_TILE_HEIGHT;
    return graphics_draw_sprite(index, x, y);
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

    graphics_collection_start();

    ESP_LOGI(__FUNCTION__, "Graphics system initialized. Free heap is %d.",
             esp_get_free_heap_size());
}
