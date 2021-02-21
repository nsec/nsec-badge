#pragma once

#include "rpg/Character.h"

namespace rpg
{

class MainCharacter : public Character
{
  public:
    MainCharacter() : Character{"main-character", 24, 32}
    {
        move(390, 190);
    }

    virtual void move(int new_scene_x, int new_scene_y) override;
    virtual void render(Viewport &viewport) override;

  private:
    int move_dx = 0;
    int move_dy = 0;
    unsigned long long move_time = 0;
};

} // namespace rpg
