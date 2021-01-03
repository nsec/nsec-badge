#pragma once

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

void graphics_draw_sprite(const char *, uint8_t, uint8_t);
void graphics_draw_tile(const char *, uint8_t, uint8_t);
void graphics_start(void);
void graphics_update_display(void);
