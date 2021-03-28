#pragma once

#include "rpg/Character.h"

namespace rpg::direction
{

constexpr int down = 0;
constexpr int left = 1;
constexpr int right = 2;
constexpr int up = 3;
constexpr int _count = 4;

} // namespace rpg::direction

namespace rpg
{

class MovingMixin : virtual public Character
{
  public:
    void set_max_distance(int x, int y)
    {
        max_distance_x = x;
        max_distance_y = y;
    }

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
    int max_distance_x = 0;
    int max_distance_y = 0;
};

} // namespace rpg
