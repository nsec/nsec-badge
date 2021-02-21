#pragma once

#include "rpg/Character.h"
#include "rpg/Viewport.h"
#include "rpg/data/SceneDataReader.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <vector>

namespace rpg
{

class Scene
{
  public:
    Scene(const char *name, int width, int height)
        : width{width}, height{height}, data_reader(name, width),
          viewport(data_reader, width, height)
    {
        characters = {};
        scene_lock = xSemaphoreCreateMutex();
    }

    void add_character(Character *character);

    void render();

    Viewport *get_viewport()
    {
        return &viewport;
    }

    bool lock()
    {
        return xSemaphoreTake(scene_lock, portMAX_DELAY) == pdTRUE;
    }

    void unlock()
    {
        xSemaphoreGive(scene_lock);
    }

  private:
    const int width;

    const int height;

    data::SceneDataReader data_reader;

    Viewport viewport;

    std::vector<Character *> characters;

    SemaphoreHandle_t scene_lock;

    void render_layer(int layer);
};

} // namespace rpg
