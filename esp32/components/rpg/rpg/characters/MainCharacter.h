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
    }

    virtual void move(GlobalCoordinates coordinates) override;
    virtual void render(Viewport &viewport) override;

  private:
    int move_dx = 0;
    int move_dy = 0;
    unsigned long long move_time = 0;

    data::BlockedDataReader &data_reader;
};

} // namespace rpg
