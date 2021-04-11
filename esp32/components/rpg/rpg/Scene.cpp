#include "rpg/Scene.h"

#include "graphics.h"

namespace rpg
{

void Scene::add_character(Character *character)
{
    characters.push_back(character);
    add_scene_object(character);
}

void Scene::add_scene_object(SceneObject *object)
{
    scene_objects.push_back(object);
}

void Scene::add_character(MainCharacter *character)
{
    assert(
        main_character == nullptr &&
        "Cannot set pointer to the main character because it is already set.");

    main_character = character;
    add_character(static_cast<Character *>(character));
}

/**
 * Search list of scene objects for an object with the requested identity.
 *
 * The first one matched object is returned. This is not enforced, but the
 * assumption is that any identity will be used by at most one object in the
 * scene.
 */
SceneObject *Scene::find_object_by_identity(SceneObjectIdentity identity)
{
    for (auto object_p : scene_objects)
        if (object_p->get_identity() == identity)
            return object_p;

    return nullptr;
}

/**
 * Find all tiles that should be treated as objects.
 *
 * The current implementation of the rendering loop treats level 4 differently
 * from the rest - it is assumed to only contain objects that the main
 * character can interact with in some way. All image tiles in level 4 must be
 * converted to an object that captures all information necessary for correct
 * rendering, which is done by this method.
 *
 * The tilemap scan is performed in the blocks of the size of the viewport in
 * order to reduce the number of read operations from the flash.
 */
void Scene::populate_objects_from_scene()
{
    for (int x_a = 0; x_a < scene_size.tile_x(); x_a += viewport_tiles_width) {
        for (int y_a = 0; y_a < scene_size.tile_y();
             y_a += viewport_tiles_height) {

            for (int x_b = 0; x_b < viewport_tiles_width; ++x_b) {
                for (int y_b = 0; y_b < viewport_tiles_height; ++y_b) {
                    if (x_a + x_b >= scene_size.tile_x())
                        continue;

                    if (y_a + y_b >= scene_size.tile_y())
                        continue;

                    auto coordinates =
                        GlobalCoordinates::tile(x_a + x_b, y_a + y_b);

                    int image = data_reader.get_image(coordinates, 4);
                    if (image) {
                        // No bounds checking, YOLO.
                        int height =
                            graphics_static_images_registry[image].height;

                        int sinkline =
                            graphics_static_images_registry[image].sinkline;

                        int width =
                            graphics_static_images_registry[image].width;

                        // Because the raw pointers to created objects are
                        // saved into the scene_objects vector, if the
                        // tile_objects vector re-allocates its internal
                        // storage, all those pointers will become invalid.
                        assert(tile_objects.size() <
                                   tile_objects_vector_reserve - 1 &&
                               "Adding any more objects to tile_objects vector "
                               "will cause it to re-allocate.");

                        tile_objects.emplace_back(coordinates, width, height,
                                                  image, sinkline);

                        add_scene_object(&(tile_objects.back()));
                    }
                }
            }
        }
    }
}

void Scene::render()
{
    if (!lock())
        return;

    graphics_clip_set(0, 0, viewport_crop_width, viewport_crop_height);
    viewport.prime_refresh_state(characters);

    for (int layer = 0; layer < 4; ++layer)
        render_layer(layer);

    // Layer 4 became scene objects.
    render_scene_objects();

    for (int layer = 5; layer < 8; ++layer)
        render_layer(layer);

    graphics_update_display();
    graphics_clip_reset();
    viewport.cache_refresh_state();
    unlock();

    for (auto object : scene_objects)
        object->post_render(viewport);
}

void Scene::render_layer(int layer)
{
    data::tilemap_word_t dependency;
    data::tilemap_word_t image;

    for (int y = -viewport_prepend_rows; y < 0; ++y) {
        for (int x = -viewport_prepend_cols; x < viewport_tiles_width; ++x) {
            auto local = LocalCoordinates::tile(x, y);
            auto global = viewport.to_global(local);

            dependency = data_reader.get_dependency(global);
            if (dependency == 0)
                continue;

            image = data_reader.get_image(global, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(local))
                continue;

            auto screen = viewport.to_screen(local);
            graphics_draw_from_library(image, screen.x(), screen.y());
        }
    }

    for (int y = 0; y < viewport_tiles_height; ++y) {
        for (int x = -viewport_prepend_cols; x < 0; ++x) {
            auto local = LocalCoordinates::tile(x, y);
            auto global = viewport.to_global(local);

            dependency = data_reader.get_dependency(global);
            if (dependency == 0)
                continue;

            image = data_reader.get_image(global, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(local))
                continue;

            auto screen = viewport.to_screen(local);
            graphics_draw_from_library(image, screen.x(), screen.y());
        }

        for (int x = 0; x < viewport_tiles_width; ++x) {
            auto local = LocalCoordinates::tile(x, y);
            auto global = viewport.to_global(local);

            image = data_reader.get_image(global, layer);
            if (image == 0)
                continue;

            if (!viewport.tile_needs_refresh(local))
                continue;

            auto screen = viewport.to_screen(local);
            graphics_draw_from_library(image, screen.x(), screen.y());
        }
    }
}

/**
 * Render all scene objects in the correct order.
 *
 * The main character is rendered each time because it is expected to move and
 * animate constantly. Other objects are rendered only if they are located on a
 * tile that is marked for refresh. This allows to avoid unnecessary rendering
 * of parts of the screen that do not have anything happening (e.g. parts of
 * the terrain.)
 */
void Scene::render_scene_objects()
{
    sort_scene_objects();

    for (auto object : scene_objects) {
        if (object != main_character) {
            auto local = viewport.to_local(object->get_coordinates());
            if (!viewport.tile_needs_refresh(local))
                continue;
        }

        object->render(viewport);
    }
}

/**
 * Put objects into correct rendering order.
 *
 * The ordering is determined by the sinkline property of each object, which
 * creates a simple scene depth simulation.
 *
 * A custom implementation of the insertion sort is used because of its near
 * linear performance on small, almost sorted sets, which is the case here.
 */
void Scene::sort_scene_objects()
{
    for (auto i = scene_objects.begin(); i != scene_objects.end(); ++i) {
        if (i == scene_objects.begin())
            continue;

        auto j = i;
        auto k = i - 1;

        int j_sinkline = (*j)->get_coordinates().y() + (*j)->get_sinkline();

        for (; k >= scene_objects.begin(); --j, --k) {
            int k_sinkline = (*k)->get_coordinates().y() + (*k)->get_sinkline();

            if (j_sinkline >= k_sinkline)
                break;

            auto temp = *j;
            *j = *k;
            *k = temp;
        }
    }
}

} // namespace rpg
