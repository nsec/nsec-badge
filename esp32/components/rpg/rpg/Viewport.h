#pragma once

extern "C" {
#include "graphics.h"
}

namespace rpg
{

struct local_coordinates_t {
    int local_tile_x;
    int local_tile_y;
    int screen_x;
    int screen_y;
};

struct tile_coordinates_t {
    int screen_x;
    int screen_y;
    int tile_x;
    int tile_y;
};

class Viewport
{
  public:
    Viewport(int scene_width, int scene_height)
        : scene_width{scene_width}, scene_height{scene_height},
          scene_width_tiles{scene_width / DISPLAY_TILE_WIDTH},
          scene_height_tiles{scene_height / DISPLAY_TILE_HEIGHT}, x{0}, y{0},
          tile_x{0}, tile_y{0}
    {
    }

    // FIXME:
    // error: 'CONFIG_HEIGHT' was not declared in this scope
    // note: in expansion of macro 'DISPLAY_PHY_HEIGHT'
    // note: in expansion of macro 'DISPLAY_WIDTH'
    static const int width = 240;
    static const int width_tiles = 10;
    static const int height = 210;
    static const int height_tiles = 9;

    local_coordinates_t get_local_coordinates(int scene_x, int scene_y);
    tile_coordinates_t get_tile_coordinates(int local_tile_x, int local_tile_y);
    void move_to_tile(int new_tile_x, int new_tile_y);

  private:
    const int scene_width;
    const int scene_height;
    const int scene_width_tiles;
    const int scene_height_tiles;
    int x;
    int y;
    int tile_x;
    int tile_y;
};

} // namespace rpg
