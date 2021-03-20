#pragma once

#include "rpg/Character.h"

namespace rpg
{

class MovingMixin : virtual public Character
{
  protected:
    int get_moving_direction()
    {
        return direction;
    }

    void moving_mixin();

    void set_moving_direction(int new_direction)
    {
        direction = new_direction;
    }

  private:
    int direction = 0;
    int move_counter = 0;
};

} // namespace rpg
