#include "rpg/characters/MovingMixin.h"

namespace rpg
{

void MovingMixin::moving_mixin()
{
    int delta = (get_animation_step() - move_counter) * 2;
    if (delta > 5) {
        delta = 5;
    }

    if (timer.has_expired(get_animation_step()))
        return;

    auto coordinates = get_coordinates();

    switch (direction) {
    case 0:
        coordinates.change_xy_by(-delta, 0);
        break;

    case 1:
        coordinates.change_xy_by(delta, 0);
        break;

    case 2:
        coordinates.change_xy_by(0, -delta);
        break;

    case 3:
    default:
        coordinates.change_xy_by(0, delta);
    }

    if (!move(coordinates))
        direction = get_animation_step() % 4;

    move_counter = get_animation_step();
}

} // namespace rpg
