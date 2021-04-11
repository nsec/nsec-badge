#pragma once

#include "rpg/Character.h"
#include "rpg/Coordinates.h"
#include "rpg/SceneObject.h"
#include "rpg/TileObject.h"
#include "rpg/Viewport.h"
#include "rpg/characters/MainCharacter.h"
#include "rpg/data/SceneDataReader.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <vector>

namespace rpg
{

class Scene
{
    constexpr static unsigned int characters_vector_reserve = 25;
    constexpr static unsigned int scene_objects_extra_reserve = 10;
    constexpr static unsigned int tile_objects_vector_reserve = 100;

  public:
    Scene(const char *name, GlobalCoordinates scene_size)
        : scene_size{scene_size}, data_reader(name, scene_size),
          viewport(data_reader, scene_size)
    {
        scene_lock = xSemaphoreCreateMutex();

        // Allocate a reasonable amount of space once.
        characters.reserve(characters_vector_reserve);
        scene_objects.reserve(characters_vector_reserve +
                              scene_objects_extra_reserve +
                              tile_objects_vector_reserve);
        tile_objects.reserve(tile_objects_vector_reserve);

        populate_objects_from_scene();
    }

    void add_character(Character *character);

    void add_character(MainCharacter *character);

    void add_scene_object(SceneObject *object);

    SceneObject *find_object_by_identity(SceneObjectIdentity identity);

    void render();

    std::vector<Character *> get_characters()
    {
        return characters;
    }

    Viewport &get_viewport()
    {
        return viewport;
    }

    bool is_paused()
    {
        return paused;
    }

    bool lock()
    {
        return xSemaphoreTake(scene_lock, portMAX_DELAY) == pdTRUE;
    }

    MainCharacter *get_main_character()
    {
        assert(main_character != nullptr &&
               "Scene does not have the main character.");
        return main_character;
    }

    void pause()
    {
        paused = true;
    }

    void unlock()
    {
        xSemaphoreGive(scene_lock);
    }

    void unpause()
    {
        paused = false;
    }

  private:
    bool paused = false;

    const GlobalCoordinates scene_size;

    data::SceneDataReader data_reader;

    Viewport viewport;

    std::vector<Character *> characters{};

    MainCharacter *main_character = nullptr;

    /**
     * All static and dymanic scene objects, in the correct rendering order.
     */
    std::vector<SceneObject *> scene_objects{};

    /**
     * Storage for dynamically constructed TileObjects.
     *
     * This vector is filled by populate_objects_from_scene() method based on
     * the definition of the scene.
     */
    std::vector<TileObject> tile_objects{};

    SemaphoreHandle_t scene_lock;

    /**
     * Add references to the objects vector from the whole scene.
     *
     * Do the initial scan of the whole scene definition file to discover any
     * images that should become scene objects with correct rendering order,
     * and add them to the objects vector.
     */
    void populate_objects_from_scene();

    void render_layer(int layer);

    void render_scene_objects();

    void sort_scene_objects();
};

} // namespace rpg
