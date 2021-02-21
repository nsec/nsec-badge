#pragma once

#include "rpg/Character.h"
#include "rpg/data/SceneDataReader.h"

extern "C" {
#include "graphics.h"
}

#include <array>
#include <vector>

namespace rpg
{

constexpr int viewport_width = 240;
constexpr int viewport_tiles_width = 10;
constexpr int viewport_height = 210;
constexpr int viewport_tiles_height = 9;

constexpr int viewport_prepend_cols = 2;
constexpr int viewport_prepend_rows = 2;

using viewport_refresh_state_t =
    std::array<bool, ((viewport_prepend_cols + viewport_tiles_width) *
                      (viewport_prepend_rows + viewport_tiles_height))>;

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
          tile_x{0}, tile_y{0}, needs_full_refresh{true}
    {
        refresh_state = new viewport_refresh_state_t();
    }

    ~Viewport()
    {
        delete[] refresh_state;
    }

    void cache_refresh_state();

    local_coordinates_t get_local_coordinates(int scene_x, int scene_y);

    tile_coordinates_t get_tile_coordinates(int local_tile_x, int local_tile_y);

    void move_to_tile(int new_tile_x, int new_tile_y);

    void prime_refresh_state(data::SceneDataReader &data_reader,
                             const std::vector<Character *> &characters);

    bool tile_needs_refresh(int tile_x, int tile_y) const;

  private:
    const int scene_width;
    const int scene_height;
    const int scene_width_tiles;
    const int scene_height_tiles;
    int x;
    int y;
    int tile_x;
    int tile_y;
    bool needs_full_refresh;
    viewport_refresh_state_t *refresh_state;

    void prime_refresh_state_tile(data::SceneDataReader &data_reader,
                                  int tile_x, int tile_y);
};

} // namespace rpg
