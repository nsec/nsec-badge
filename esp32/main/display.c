#include "esp_spiffs.h"
#include "esp_vfs.h"

#include "decode_image.h"
#include "st7789.h"

TFT_t display_dev;

void display_jpeg_file(char *file, int width, int height)
{
    pixel_s **pixels;
    uint16_t imageWidth;
    uint16_t imageHeight;

    esp_err_t err =
        decode_image(&pixels, file, width, height, &imageWidth, &imageHeight);
    if (err != ESP_OK) {
        return;
    }

    uint16_t _width = width;
    uint16_t _cols = 0;
    if (width > imageWidth) {
        _width = imageWidth;
        _cols = (width - imageWidth) / 2;
    }

    uint16_t _height = height;
    uint16_t _rows = 0;
    if (height > imageHeight) {
        _height = imageHeight;
        _rows = (height - imageHeight) / 2;
    }

    uint16_t *colors = (uint16_t *)malloc(sizeof(uint16_t) * _width);

    for (int y = 0; y < _height; y++) {
        for (int x = 0; x < _width; x++) {
            pixel_s pixel = pixels[y][x];
            colors[x] = rgb565_conv(pixel.red, pixel.green, pixel.blue);
        }
        lcdDrawMultiPixels(&display_dev, _cols, y + _rows, _width, colors);
    }

    free(colors);
    release_image(&pixels, width, height);
}

void display_init(void)
{
    esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                  .partition_label = NULL,
                                  .max_files = 8,
                                  .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        abort();
    }

    spi_master_init(&display_dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO,
                    CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO,
                    CONFIG_BL_GPIO);

    lcdInit(&display_dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX,
            CONFIG_OFFSETY);
    lcdFillScreen(&display_dev, BLUE);

    display_jpeg_file("/spiffs/nsec.jpeg", CONFIG_WIDTH, CONFIG_HEIGHT);
}
