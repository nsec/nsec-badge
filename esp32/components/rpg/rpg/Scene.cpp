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

void Scene::add_character(MainCharacter *character)
{
    assert(
        main_character == nullptr &&
        "Cannot set pointer to the main character because it is already set.");

    main_character = character;
    add_character(static_cast<Character *>(character));
}

void Scene::render()
{
    if (!lock())
        return;

    int character_sinkline =
        viewport
            .get_local_coordinates(main_character->get_scene_x(),
                                   main_character->get_scene_y())
            .screen_y +
        main_character->get_ground_base_y();

    graphics_clip_set(0, 0, viewport_crop_width, viewport_crop_height);

    tile_coordinates_t coordinates = viewport.get_tile_coordinates(0, 0);
    data_reader.read_tilemap(coordinates.tile_x, coordinates.tile_y);

    viewport.prime_refresh_state(characters);

    for (int layer = 0; layer < 4; ++layer)
        render_layer(layer, 0, false);

    bool repeat = render_layer(4, character_sinkline, false);

    for (auto character : characters)
        character->render(viewport);

    if (repeat)
        render_layer(4, character_sinkline, true);

    for (int layer = 5; layer < 8; ++layer)
        render_layer(layer, 0, false);

    graphics_update_display();
    graphics_clip_reset();
    viewport.cache_refresh_state();
    unlock();
}

bool Scene::render_layer(int layer, int sinkline_check, bool sinkline_repeat)
{
    bool repeat = false;
    int sinkline = 0;

    tile_coordinates_t coordinates;
    data::tilemap_word_t dependency;
    data::tilemap_word_t image;

    for (int y = -viewport_prepend_rows; y < 0; ++y) {
        for (int x = -viewport_prepend_cols; x < viewport_tiles_width; ++x) {
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
        for (int x = -viewport_prepend_cols; x < 0; ++x) {
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

            if (sinkline_check) {
                sinkline = coordinates.screen_y +
                           graphics_get_sinkline_from_library(image);

                if (sinkline >= sinkline_check && !sinkline_repeat) {
                    repeat = true;
                    continue;
                }

                if (sinkline < sinkline_check && sinkline_repeat) {
                    continue;
                }
            }

            graphics_draw_from_library(image, coordinates.screen_x,
                                       coordinates.screen_y);
        }
    }

    return repeat;
}

} // namespace rpg
