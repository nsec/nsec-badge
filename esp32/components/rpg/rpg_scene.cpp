#include "rpg_scene.h"

#include "palette.h"

namespace rpg
{

void Scene::update()
{
    tilemap_t tilemap{};

    // Render all tiles in one layer at a time (instead of rendering all layers
    // in one tile at a time) to ensure correct display of multi-tile overlayed
    // images. TODO: switch ordering of fields in the binary scene definition
    // file to reduce number of file reads.
    for (int layer = 0; layer < 8; layer++) {
        // TODO: Implement using some sort of standard C++ iterator.
        for (auto ref = viewport.begin(); ref < viewport.end(); ++ref) {
            auto coordinates = viewport.get_tile_coordinates(ref);
            data_reader.read_tilemap(coordinates.tile_x, coordinates.tile_y,
                                     tilemap);

            if (tilemap[layer] != 0) {
                graphics_draw_from_library(tilemap[layer], coordinates.screen_x,
                                           coordinates.screen_y);
            }
        }
    }

    graphics_update_display();
}

tile_coordinates_ref Viewport::begin()
{
    return 0;
}

tile_coordinates_ref Viewport::end()
{
    return DISPLAY_TILES_X * DISPLAY_TILES_Y;
}

tile_coordinates_t Viewport::get_tile_coordinates(tile_coordinates_ref ref)
{
    int local_x = (ref % DISPLAY_TILES_X);
    int local_y = (ref / DISPLAY_TILES_X);

    return {
        .screen_x = (local_x * DISPLAY_TILE_WIDTH),
        .screen_y = (local_y * DISPLAY_TILE_HEIGHT),
        .tile_x = local_x + x,
        .tile_y = local_y + y,
    };
}

void Viewport::move(int new_x, int new_y)
{
    x = new_x;
    y = new_y;
}

} // namespace rpg
