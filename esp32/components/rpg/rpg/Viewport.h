#pragma once

#include "rpg/Character.h"
#include "rpg/Coordinates.h"
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

    void mark_for_full_refresh()
    {
        needs_full_refresh = true;
    }

    /**
     * Move the viewport to a new position on the scene.
     */
    bool move(GlobalCoordinates coordinates);

    /**
     * Adjust viewport relative to its current position.
     */
    bool move_relative(GlobalCoordinates coordinates);

    void prime_refresh_state(const std::vector<Character *> &characters);

    bool tile_needs_refresh(int tile_x, int tile_y) const;

    /**
     * Backconvert internal viewport coordinates to global scene coordinates.
     */
    GlobalCoordinates to_global(LocalCoordinates local);

    /**
     * Convert global scene coordinates to internal viewport coordinates.
     */
    LocalCoordinates to_local(GlobalCoordinates global);

    /**
     * Convert scene coordinates to an actual position on the screen.
     *
     * Any position in the global coordinates system is accepted, so converted
     * coordinates may actually be off-screen.
     */
    ScreenCoordinates to_screen(LocalCoordinates local);

    /**
     * Convert internal viewport coordinates to a position on the screen.
     */
    ScreenCoordinates to_screen(GlobalCoordinates global);

  private:
    const int scene_width;
    const int scene_height;
    const int scene_width_tiles;
    const int scene_height_tiles;

    data::SceneBaseDataReader &data_reader;

    int offset_move_x = 0;
    int offset_move_y = 0;

    bool needs_full_refresh = true;

    viewport_refresh_state_t *refresh_state;

    void prime_refresh_state_tile(int tile_x, int tile_y);
};

} // namespace rpg
