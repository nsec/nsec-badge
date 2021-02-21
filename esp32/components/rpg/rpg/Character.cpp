#include "rpg/Character.h"

namespace rpg
{

void Character::render(Viewport &viewport)
{
    rendered_scene_x = scene_x;
    rendered_scene_y = scene_y;
}

} // namespace rpg
