#pragma once

#include "rpg/Character.h"
#include "rpg/Viewport.h"
#include "rpg/data/SceneDataReader.h"

#include <vector>

namespace rpg
{

class Scene
{
  public:
    Scene(const char *name, int width, int height)
        : width{width}, height{height}, data_reader(name, width),
          viewport(width, height)
    {
        characters = {};
    }

    void add_character(Character *character);

    void update();

    Viewport *get_viewport()
    {
        return &viewport;
    }

  private:
    const int width;

    const int height;

    data::SceneDataReader data_reader;

    Viewport viewport;

    std::vector<Character *> characters;
};

} // namespace rpg
