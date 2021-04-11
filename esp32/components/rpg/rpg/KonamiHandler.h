#pragma once

#include "rpg/Scene.h"
#include "rpg/data/BlockedDataReader.h"
#include "rpg/data/SceneDataReader.h"

namespace rpg
{

class KonamiHandler
{
  public:
    KonamiHandler(Scene &scene, data::BlockedDataReader &blocked_data_reader)
        : blocked_data_reader{blocked_data_reader}, scene{scene}
    {
    }

    void patch();

  private:
    data::BlockedDataReader &blocked_data_reader;
    Scene &scene;
};

} // namespace rpg
