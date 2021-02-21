#pragma once

#include "rpg_data.h"

extern "C" {
#include "graphics.h"
}

#include <tuple>
#include <vector>

namespace rpg
{

struct tile_coordinates_t {
    int screen_x;
    int screen_y;
    int tile_x;
    int tile_y;
};

class Scene;

class Viewport
{

  public:
    Viewport(int scene_width, int scene_height)
        : scene_width{scene_width}, scene_height{scene_height}, x{0}, y{0}
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

    tile_coordinates_t get_tile_coordinates(int local_tile_x, int local_tile_y);
    void move(int new_x, int new_y);

  private:
    const int scene_width;
    const int scene_height;
    int x;
    int y;
};

class Scene
{
  public:
    Scene(const char *name, int width, int height)
        : width{width}, height{height}, data_reader(name, width),
          viewport(width, height)
    {
    }

    void update();

    Viewport *get_viewport()
    {
        return &viewport;
    }

  private:
    const int width;

    const int height;

    SceneDataReader data_reader;

    Viewport viewport;
};

} // namespace rpg
