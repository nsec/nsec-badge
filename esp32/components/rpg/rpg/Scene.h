#pragma once

#include "rpg/Character.h"
#include "rpg/Coordinates.h"
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
  public:
    Scene(const char *name, GlobalCoordinates scene_size)
        : scene_size{scene_size}, data_reader(name, scene_size),
          viewport(data_reader, scene_size)
    {
        characters = {};
        scene_lock = xSemaphoreCreateMutex();
    }

    void add_character(Character *character);

    void add_character(MainCharacter *character);

    void render();

    Viewport &get_viewport()
    {
        return viewport;
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

    void unlock()
    {
        xSemaphoreGive(scene_lock);
    }

  private:
    const GlobalCoordinates scene_size;

    data::SceneDataReader data_reader;

    Viewport viewport;

    std::vector<Character *> characters;

    MainCharacter *main_character = nullptr;

    SemaphoreHandle_t scene_lock;

    bool render_layer(int layer, int sinkline_check, bool sunkline_repeat);
};

} // namespace rpg
