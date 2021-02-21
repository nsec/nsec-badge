#pragma once

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
    }

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
};

} // namespace rpg
