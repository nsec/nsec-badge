#include "rpg/Viewport.h"

namespace rpg
{

local_coordinates_t Viewport::get_local_coordinates(int scene_x,
                                                           int scene_y)
{
    int screen_x = scene_x - x;
    int screen_y = scene_y - y;

    int local_tile_x = screen_x / DISPLAY_TILE_WIDTH;
    if (screen_x < 0)
        local_tile_x--;

    int local_tile_y = screen_y / DISPLAY_TILE_HEIGHT;
    if (screen_y < 0)
        local_tile_y--;

    return {
        local_tile_x = local_tile_x,
        local_tile_y = local_tile_y,
        screen_x = screen_x,
        screen_y = screen_y,
    };
}

tile_coordinates_t Viewport::get_tile_coordinates(int local_tile_x,
                                                         int local_tile_y)
{
    return {
        .screen_x = (local_tile_x * DISPLAY_TILE_WIDTH),
        .screen_y = (local_tile_y * DISPLAY_TILE_HEIGHT),
        .tile_x = local_tile_x + tile_x,
        .tile_y = local_tile_y + tile_y,
    };
}

void Viewport::move_to_tile(int new_tile_x, int new_tile_y)
{
    if (new_tile_x >= 0 && new_tile_x < scene_width_tiles) {
        tile_x = new_tile_x;
        x = new_tile_x * DISPLAY_TILE_WIDTH;
    }

    if (new_tile_y >= 0 && new_tile_y < scene_height_tiles) {
        tile_y = new_tile_y;
        y = new_tile_y * DISPLAY_TILE_HEIGHT;
    }
}

}
