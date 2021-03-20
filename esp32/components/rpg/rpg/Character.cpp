#include "rpg/Character.h"

#include "rpg/Viewport.h"

namespace rpg
{

bool Character::is_visible(Viewport &viewport)
{
    auto coordinates = GlobalCoordinates::xy(scene_x, scene_y);
    if (viewport.is_visible(coordinates))
        return true;

    coordinates.change_xy_by(width, 0);
    if (viewport.is_visible(coordinates))
        return true;

    coordinates.change_xy_by(0, height);
    if (viewport.is_visible(coordinates))
        return true;

    coordinates.change_xy_by(-width, 0);
    if (viewport.is_visible(coordinates))
        return true;

    return false;
}

void Character::render(Viewport &viewport)
{
    rendered_scene_x = scene_x;
    rendered_scene_y = scene_y;
}

} // namespace rpg
