#include "rpg/characters/MovingMixin.h"

namespace rpg
{

void MovingMixin::moving_mixin()
{
    int delta = (get_animation_step() - move_counter) * 2;
    if (delta > 10) {
        delta = 10;
    }

    if (timer.has_expired(get_animation_step()))
        return;

    auto coordinates = get_coordinates();

    switch (direction) {
    case direction::left:
        coordinates.change_xy_by(-delta, 0);
        break;

    case direction::right:
        coordinates.change_xy_by(delta, 0);
        break;

    case direction::up:
        coordinates.change_xy_by(0, -delta);
        break;

    case direction::down:
    default:
        coordinates.change_xy_by(0, delta);
    }

    if (max_distance_x > 0 && max_distance_y > 0) {
        auto limit1 = get_initial_coordinates();
        limit1.change_xy_by(-max_distance_x, -max_distance_y);

        auto limit2 = get_initial_coordinates();
        limit2.change_xy_by(max_distance_x, max_distance_y);

        if (coordinates.x() < limit1.x()) {
            direction = direction::right;
            return moving_mixin();
        }

        if (coordinates.y() < limit1.y()) {
            direction = direction::down;
            return moving_mixin();
        }

        if (coordinates.x() > limit2.x()) {
            direction = direction::left;
            return moving_mixin();
        }

        if (coordinates.y() > limit2.y()) {
            direction = direction::up;
            return moving_mixin();
        }
    }

    if (!move(coordinates))
        direction = get_animation_step() % direction::_count;

    move_counter = get_animation_step();
}

} // namespace rpg
