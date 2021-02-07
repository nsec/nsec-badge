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
#include "images_registry.h"
#include "palette.h"

#define TJPGD_WORK_SZ 3100

typedef struct {
    FILE *fp;
    int offset_x;
    int offset_y;
} jpeg_session_device;

/* In-memory framebuffer with the contents of the screen. */
static pixel_t *display_buffer = NULL;

/* Active drawing clip coordinates. */
static int display_clip[4] = {};

/* Device state object for the display driver. */
static TFT_t display_device = {};

/* Bitfield index of updated rows that need to be flushed. */
static uint8_t display_rows_hot[DISPLAY_WIDTH / 8] = {};

/* In-memory cache for 'fast' images. */
static uint8_t *library_maps_fast = NULL;

/* File pointer to the concatenated image maps in the library. */
static FILE *library_maps_fp = NULL;

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

    // fopen() is a very expensive operation on SPIFFS. Open once and use the
    // same file pointer for all sprite image reads.
    library_maps_fp = fopen("/spiffs/library/maps", "r");
    if (!library_maps_fp) {
        abort();
    }

    // Copy the most used 'fast' images into a heap buffer to reduce the number
    // of flash memory reads.
    fseek(library_maps_fp, 0, SEEK_CUR);
    fread(library_maps_fast, 1, IMAGE_REGISTRY_FAST_SIZE, library_maps_fp);
}

// Display functions.

/**
 * Enable drawing on the whole screen area.
 */
void graphics_clip_reset()
{
    display_clip[0] = 0;
    display_clip[1] = 0;
    display_clip[2] = DISPLAY_WIDTH;
    display_clip[3] = DISPLAY_HEIGHT;
}

/**
 * Set the active drawing clip on the screen.
 *
 * Any pixels drawn outside of the clip area will be ignored.
 */
void graphics_clip_set(int x1, int y1, int x2, int y2)
{
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 > DISPLAY_WIDTH)
        x2 = DISPLAY_WIDTH;
    if (y2 > DISPLAY_HEIGHT)
        y2 = DISPLAY_HEIGHT;

    display_clip[0] = x1;
    display_clip[1] = y1;
    display_clip[2] = x2;
    display_clip[3] = y2;
}

/**
 * Put a single high color (RGB565) pixel into the framebuffer.
 *
 * The hight and low bytes of the pixel value must already be swapped. This is
 * an optimisation used to achieve the same memory layout that is used by the
 * screen.
 */
static inline void graphics_put_pixel(int x, int y, pixel_t new_pixel)
{
    if (x < display_clip[0] || y < display_clip[1])
        return;

    if (x >= display_clip[2] || y >= display_clip[3])
        return;

    // Zero pixel is a special value for transparency.
    if (new_pixel == 0)
        return;

    // Store columns in memory in the reverse order to achieve the correct
    // mirrored appearance of the image on the display that is mounted
    // sideways.
    unsigned int rx = DISPLAY_WIDTH - x - 1;

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
static inline void graphics_put_pixel_rgb(int x, int y, uint8_t r, uint8_t g,
                                          uint8_t b)
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

    library_maps_fast = calloc(IMAGE_REGISTRY_FAST_SIZE, sizeof(char));
    assert(library_maps_fast != NULL);

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
        int offset_y = session_device->offset_y + y;
        if (offset_y >= display_clip[3])
            break;

        for (int x = rect->left; x <= rect->right; x++, rgb += 3) {
            int offset_x = session_device->offset_x + x;
            if (offset_x >= display_clip[2])
                continue;

            graphics_put_pixel_rgb(offset_x, offset_y, rgb[0], rgb[1], rgb[2]);
        }
    }

    return 1;
}

/**
 * Draw a single sprite image from the library into the display buffer.
 */
void graphics_draw_from_library(int index, int x, int y)
{
    // The 0th entry represents an "empty" image, so nothing can be rendered in
    // this case.
    if (index == 0)
        return;

    char filepath[64];

    switch (graphics_static_images_registry[index].type) {
    case IMAGE_REGISTRY_JPEG:
        sprintf(filepath, "/spiffs/library/%s.jpeg",
                graphics_static_images_registry[index].filename);

        graphics_draw_jpeg(filepath, x, y);
        return;

    case IMAGE_REGISTRY_FAST:
    case IMAGE_REGISTRY_MAP:
        graphics_draw_sprite(&(graphics_static_images_registry[index]), x, y);
        return;

    default:
        ESP_LOGE(__FUNCTION__, "Unknown type of graphics library sprite %d.",
                 index);
        abort();
    }
}

/**
 * Draw a single JPEG image sprite into the display buffer.
 */
void graphics_draw_jpeg(const char *name, int x, int y)
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
 * Draw a single color mapped image sprite into the display buffer.
 */
void graphics_draw_sprite(const ImagesRegistry_t *sprite, int x, int y)
{
    const uint16_t *palette;
    switch (sprite->palette) {
    case 0:
        palette = graphics_static_palette_0;
        break;

    case 1:
        palette = graphics_static_palette_1;
        break;

    default:
        ESP_LOGE(__FUNCTION__, "Sprite image uses unknown palette %d.",
                 sprite->palette);
        abort();
    }

    int width = sprite->width;
    int height = sprite->height;
    unsigned int offset = sprite->map_offset;
    unsigned int total_size = width * height;

    if (sprite->type == IMAGE_REGISTRY_FAST) {
        for (int iy = 0; iy < height; ++iy) {
            for (int ix = 0; ix < width; ++ix) {
                graphics_put_pixel(
                    x + ix, y + iy,
                    palette[library_maps_fast[offset + (iy * width) + ix]]);
            }
        }
    } else {
        fseek(library_maps_fp, offset, SEEK_SET);

        uint8_t buffer[576];
        for (int i = 0; i < total_size; i += 576) {
            fread(buffer, 1, 576, library_maps_fp);

            for (int j = 0; j < 576; ++j) {
                graphics_put_pixel((x + ((i + j) % width)),
                                   (y + ((i + j) / width)), palette[buffer[j]]);
            }
        }
    }
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

    graphics_clip_reset();
    graphics_display_buffers_allocate();

    display_spi_bus_init();
    lcdInit(&display_device, DISPLAY_PHY_WIDTH, DISPLAY_PHY_HEIGHT, 0, 0);
    lcdFillScreen(&display_device, BLUE);
    gpio_set_level(DISPLAY_SPI_BL, 1);

    graphics_collection_start();

    ESP_LOGI(__FUNCTION__, "Graphics system initialized. Free heap is %d.",
             esp_get_free_heap_size());
}
