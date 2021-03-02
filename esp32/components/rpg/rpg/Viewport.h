#pragma once

#include "rpg/Character.h"
#include "rpg/data/SceneDataReader.h"
#include "rpg_const.h"

#include "graphics.h"

#include <array>
#include <vector>

namespace rpg
{

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
    Viewport(data::SceneBaseDataReader &data_reader, int scene_width,
             int scene_height)
        : scene_width{scene_width}, scene_height{scene_height},
          scene_width_tiles{scene_width / DISPLAY_TILE_WIDTH},
          scene_height_tiles{scene_height / DISPLAY_TILE_HEIGHT},
          data_reader{data_reader}
    {
        refresh_state = new viewport_refresh_state_t();
    }

    ~Viewport()
    {
        delete[] refresh_state;
    }

    void cache_refresh_state();

    void center(int scene_x, int scene_y);

    local_coordinates_t get_local_coordinates(int scene_x, int scene_y);

    tile_coordinates_t get_tile_coordinates(int local_tile_x, int local_tile_y);

    void mark_for_full_refresh()
    {
        needs_full_refresh = true;
    }

    bool move(int new_x, int new_y);

    bool move_relative(int dx, int dy);

    bool move_to_tile(int new_tile_x, int new_tile_y);

    bool move_to_tile_relative(int tile_dx, int tile_dy);

    void prime_refresh_state(const std::vector<Character *> &characters);

    bool tile_needs_refresh(int tile_x, int tile_y) const;

  private:
    const int scene_width;
    const int scene_height;
    const int scene_width_tiles;
    const int scene_height_tiles;

    data::SceneBaseDataReader &data_reader;

    int x = 0;
    int y = 0;
    int tile_x = 0;
    int tile_y = 0;
    bool needs_full_refresh = true;

    viewport_refresh_state_t *refresh_state;

    void prime_refresh_state_tile(int tile_x, int tile_y);
};

} // namespace rpg
