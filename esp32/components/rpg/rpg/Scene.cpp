#include "rpg/Scene.h"

extern "C" {
#include "graphics.h"
}

namespace rpg
{

void Scene::add_character(Character *character)
{
    characters.push_back(character);
}

void Scene::render()
{
    if (!lock())
        return;

    graphics_clip_set(0, 0, viewport_width, viewport_height);

    tile_coordinates_t coordinates = viewport.get_tile_coordinates(0, 0);
    data_reader.read_tilemap(coordinates.tile_x, coordinates.tile_y);

    viewport.prime_refresh_state(characters);

    for (int layer = 0; layer < 5; ++layer)
        render_layer(layer);

    for (auto character : characters)
        character->render(viewport);

    for (int layer = 5; layer < 8; ++layer)
        render_layer(layer);

    graphics_update_display();
    graphics_clip_reset();
    viewport.cache_refresh_state();
    unlock();
}

void Scene::render_layer(int layer)
{
    tile_coordinates_t coordinates;
    data::tilemap_word_t dependency;
    data::tilemap_word_t image;

    for (int y = -data::tilemap_read_lines_extra; y < 0; ++y) {
        for (int x = -data::tilemap_cell_extra; x < viewport_tiles_width; ++x) {
            dependency = data_reader.get_dependency(x, y);
            if (dependency == 0)
                continue;

            image = data_reader.get_image(x, y, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(x, y))
                continue;

            coordinates = viewport.get_tile_coordinates(x, y);
            graphics_draw_from_library(image, coordinates.screen_x,
                                       coordinates.screen_y);
        }
    }

    for (int y = 0; y < viewport_tiles_height; ++y) {
        for (int x = -data::tilemap_cell_extra; x < 0; ++x) {
            dependency = data_reader.get_dependency(x, y);
            if (dependency == 0)
                continue;

            image = data_reader.get_image(x, y, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(x, y))
                continue;

            coordinates = viewport.get_tile_coordinates(x, y);
            graphics_draw_from_library(image, coordinates.screen_x,
                                       coordinates.screen_y);
        }

        for (int x = 0; x < viewport_tiles_width; ++x) {
            image = data_reader.get_image(x, y, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(x, y))
                continue;

            coordinates = viewport.get_tile_coordinates(x, y);
            graphics_draw_from_library(image, coordinates.screen_x,
                                       coordinates.screen_y);
        }
    }
}

} // namespace rpg
