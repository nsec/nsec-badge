#include "rpg/Scene.h"

extern "C" {
#include "graphics.h"
}

namespace rpg
{

void Scene::update()
{
    graphics_clip_set(0, 0, viewport.width, viewport.height);

    tile_coordinates_t coordinates;
    data::tilemap_word_t dependency;
    data::tilemap_word_t image;

    coordinates = viewport.get_tile_coordinates(0, 0);
    data_reader.read_tilemap(coordinates.tile_x, coordinates.tile_y);

    for (int layer = 0; layer < 8; ++layer) {
        for (int y = -data::tilemap_read_lines_extra; y < 0; ++y) {
            for (int x = -data::tilemap_cell_extra; x < viewport.width_tiles; ++x) {
                dependency = data_reader.get_dependency(x, y);
                if (dependency == 0) {
                    continue;
                }

                image = data_reader.get_image(x, y, layer);
                if (image == 0) {
                    continue;
                }

                coordinates = viewport.get_tile_coordinates(x, y);
                graphics_draw_from_library(image, coordinates.screen_x,
                                           coordinates.screen_y);
            }
        }

        for (int y = 0; y < viewport.height_tiles; ++y) {
            for (int x = -data::tilemap_cell_extra; x < 0; ++x) {
                dependency = data_reader.get_dependency(x, y);
                if (dependency == 0) {
                    continue;
                }

                image = data_reader.get_image(x, y, layer);
                if (image == 0) {
                    continue;
                }

                coordinates = viewport.get_tile_coordinates(x, y);
                graphics_draw_from_library(image, coordinates.screen_x,
                                           coordinates.screen_y);
            }

            for (int x = 0; x < viewport.width_tiles; ++x) {
                image = data_reader.get_image(x, y, layer);
                if (image == 0) {
                    continue;
                }

                coordinates = viewport.get_tile_coordinates(x, y);
                graphics_draw_from_library(image, coordinates.screen_x,
                                           coordinates.screen_y);
            }
        }
    }

    graphics_update_display();
    graphics_clip_set(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

}
