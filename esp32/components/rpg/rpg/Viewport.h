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
    Viewport(data::SceneBaseDataReader &data_reader,
             GlobalCoordinates scene_size)
        : scene_size{scene_size}, data_reader{data_reader}
    {
        refresh_state = new viewport_refresh_state_t();
    }

    ~Viewport()
    {
        delete[] refresh_state;
    }

    void cache_refresh_state();

    /**
     * Check that a point is visible on screen.
     */
    bool is_visible(GlobalCoordinates coordinate)
    {
        auto screen = to_screen(coordinate);

        if (screen.x() < 0)
            return false;

        if (screen.y() < 0)
            return false;

        if (screen.x() >= DISPLAY_WIDTH)
            return false;

        if (screen.y() >= DISPLAY_HEIGHT)
            return false;

        return true;
    }

    /**
     * Check that an object is at least partially visible.
     *
     * An object is anything that has a coordinate on the scene, non-zero width
     * and height. Even if one pixel of the object is in the viewport, the
     * function will return true.
     */
    bool is_visible(GlobalCoordinates coordinate, int width, int height);

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

    bool tile_needs_refresh(const LocalCoordinates &coordinates) const;

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
    const GlobalCoordinates scene_size;
    data::SceneBaseDataReader &data_reader;

    int offset_move_x = 0;
    int offset_move_y = 0;

    bool needs_full_refresh = true;

    viewport_refresh_state_t *refresh_state;

    void prime_refresh_state_tile(const LocalCoordinates &coordinates);
};

} // namespace rpg
