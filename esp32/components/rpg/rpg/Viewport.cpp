#include "rpg/Viewport.h"

namespace rpg
{

static inline unsigned int refresh_state_tile_offset(int tile_x, int tile_y)
{
    return (viewport_prepend_rows + tile_y) *
               (viewport_prepend_cols + viewport_tiles_width) +
           viewport_prepend_cols + tile_x;
}

void Viewport::cache_refresh_state()
{
    for (auto &i : *refresh_state)
        i = false;

    needs_full_refresh = false;
}

bool Viewport::move(GlobalCoordinates coordinates)
{
    int new_move_x = coordinates.x();
    int new_move_y = coordinates.y();

    if (new_move_x < 0)
        new_move_x = 0;

    if (new_move_x >= scene_width - viewport_width)
        new_move_x = scene_width - viewport_width - 1;

    if (new_move_y < 0)
        new_move_y = 0;

    if (new_move_y >= scene_height - viewport_height)
        new_move_y = scene_height - viewport_height - 1;

    if (new_move_x == offset_move_x && new_move_y == offset_move_y)
        return false;

    offset_move_x = new_move_x;
    offset_move_y = new_move_y;
    mark_for_full_refresh();

    return true;
}

bool Viewport::move_relative(GlobalCoordinates coordinates)
{
    coordinates.change_xy_by(offset_move_x, offset_move_y);
    return move(coordinates);
}

void Viewport::prime_refresh_state(const std::vector<Character *> &characters)
{
    if (needs_full_refresh) {
        for (auto &i : *refresh_state)
            i = true;

        return;
    }

    for (auto character_p : characters) {
        ScreenCoordinates coordinates_array[] = {
            to_screen(character_p->get_rendered_coordinates()),
            to_screen(character_p->get_coordinates()),
        };

        for (auto &coordinates : coordinates_array) {
            for (int dy = 0, y_limit = coordinates.y() +
                                       character_p->get_height() +
                                       DISPLAY_TILE_HEIGHT * 2;
                 coordinates.y() <= y_limit;
                 ++dy, y_limit -= DISPLAY_TILE_HEIGHT) {

                for (int dx = 0, x_limit = coordinates.x() +
                                           character_p->get_width() +
                                           DISPLAY_TILE_WIDTH;
                     coordinates.x() <= x_limit;
                     ++dx, x_limit -= DISPLAY_TILE_WIDTH) {

                    prime_refresh_state_tile(coordinates.tile_x() + dx,
                                             coordinates.tile_y() + dy);
                }
            }
        }
    }
}

void Viewport::prime_refresh_state_tile(int tile_x, int tile_y)
{
    if (tile_x < -viewport_prepend_cols || tile_y < -viewport_prepend_rows)
        return;

    if (tile_x >= viewport_tiles_width || tile_y >= viewport_tiles_height)
        return;

    unsigned int offset = refresh_state_tile_offset(tile_x, tile_y);
    if ((*refresh_state)[offset])
        return;

    (*refresh_state)[offset] = true;

    data::tilemap_word_t dependency =
        data_reader.get_dependency(tile_x, tile_y);

    if (dependency != 0) {
        data::tilemap_dependency_t decoded_dependency =
            data_reader.decode_dependency(dependency);

        for (int dy = decoded_dependency.backward_y; dy > -1; --dy) {
            for (int dx = decoded_dependency.backward_x; dx > -1; --dx) {
                prime_refresh_state_tile(tile_x - dx, tile_y - dy);
            }
        }

        for (int dy = 0; dy <= decoded_dependency.forward_y; ++dy) {
            for (int dx = 0; dx <= decoded_dependency.forward_x; ++dx) {
                prime_refresh_state_tile(tile_x + dx, tile_y + dy);
            }
        }
    }
}

bool Viewport::tile_needs_refresh(int tile_x, int tile_y) const
{
    if (tile_x < -viewport_prepend_cols || tile_y < -viewport_prepend_rows)
        return false;

    if (tile_x >= viewport_tiles_width || tile_y >= viewport_tiles_height)
        return false;

    return (*refresh_state)[refresh_state_tile_offset(tile_x, tile_y)];
}

GlobalCoordinates Viewport::to_global(LocalCoordinates local)
{
    int offset_global_x = local.x() + offset_move_x;
    int offset_global_y = local.y() + offset_move_y;

    // Local coordinates system is aligned to the tiles grid, so the difference
    // between the last move location and the start of the closes tile is
    // already included in the local coordinates value, and it has to be
    // substracted back.
    offset_global_x -= offset_move_x % DISPLAY_TILE_WIDTH;
    offset_global_y -= offset_move_y % DISPLAY_TILE_HEIGHT;

    return GlobalCoordinates::xy(offset_global_x, offset_global_y);
}

LocalCoordinates Viewport::to_local(GlobalCoordinates global)
{
    int offset_local_x = global.x() - offset_move_x;
    int offset_local_y = global.y() - offset_move_y;

    // Local coordinates system is aligned to the tiles grid, so the distance
    // between the last move location and the start of the closest tile must be
    // added back to the converted coordinate.
    offset_local_x += offset_move_x % DISPLAY_TILE_WIDTH;
    offset_local_y += offset_move_y % DISPLAY_TILE_HEIGHT;

    return LocalCoordinates::xy(offset_local_x, offset_local_y);
}

ScreenCoordinates Viewport::to_screen(GlobalCoordinates global)
{
    return ScreenCoordinates::xy(global.x() - offset_move_x,
                                 global.y() - offset_move_y);
}

ScreenCoordinates Viewport::to_screen(LocalCoordinates local)
{
    // The screen coordinates are offset by the difference between the last
    // viewport move location and the local coordinates that snap to the tiles
    // grid.
    return ScreenCoordinates::xy(
        local.x() - (offset_move_x % DISPLAY_TILE_WIDTH),
        local.y() - (offset_move_y % DISPLAY_TILE_HEIGHT));
}

} // namespace rpg
