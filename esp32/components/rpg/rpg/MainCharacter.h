#pragma once

#include "rpg/Character.h"
#include "rpg/data/BlockedDataReader.h"

namespace rpg
{

class MainCharacter : public Character
{
  public:
    MainCharacter(data::BlockedDataReader &data_reader)
        : Character{"main-character", 24, 32, 13, 31}, data_reader{data_reader}
    {
        move(390, 190);
    }

    virtual void move(int new_scene_x, int new_scene_y) override;
    virtual void render(Viewport &viewport) override;

  private:
    int move_dx = 0;
    int move_dy = 0;
    unsigned long long move_time = 0;

    data::BlockedDataReader &data_reader;
};

} // namespace rpg
