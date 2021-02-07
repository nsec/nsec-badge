#pragma once

#include <stdint.h>

#include "images_registry.h"

#define DISPLAY_PHY_HEIGHT CONFIG_HEIGHT
#define DISPLAY_PHY_WIDTH CONFIG_WIDTH

#define DISPLAY_HEIGHT DISPLAY_PHY_WIDTH
#define DISPLAY_WIDTH DISPLAY_PHY_HEIGHT

#define DISPLAY_SPI_BL CONFIG_BL_GPIO
#define DISPLAY_SPI_CLK CONFIG_SCLK_GPIO
#define DISPLAY_SPI_DC CONFIG_DC_GPIO
#define DISPLAY_SPI_MOSI CONFIG_MOSI_GPIO
#define DISPLAY_SPI_RST CONFIG_RESET_GPIO

#define DISPLAY_TILE_HEIGHT 24
#define DISPLAY_TILE_WIDTH 24
#define DISPLAY_TILES_X (DISPLAY_WIDTH / DISPLAY_TILE_WIDTH)
#define DISPLAY_TILES_Y (DISPLAY_HEIGHT / DISPLAY_TILE_HEIGHT)

typedef uint16_t pixel_t;

void graphics_clip_reset();
void graphics_clip_set(int, int, int, int);
void graphics_draw_from_library(int, int, int);
void graphics_draw_jpeg(const char *, int, int);
void graphics_draw_sprite(const ImagesRegistry_t *, int, int);
void graphics_start(void);
void graphics_update_display(void);
