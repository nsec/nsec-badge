#include "rpg/characters/CharacterDuck.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterDuck::post_render(Viewport &viewport)
{
    moving_mixin();
    Character::post_render(viewport);
}

void CharacterDuck::render(Viewport &viewport)
{
    auto screen = viewport.to_screen(get_coordinates());

    switch (get_moving_direction()) {
    case direction::left:
        graphics_draw_from_library(LIBRARY_IMAGE_DUCK_LEFT, screen.x(),
                                   screen.y());
        break;

    case direction::right:
    default:
        graphics_draw_from_library(LIBRARY_IMAGE_DUCK_RIGHT, screen.x(),
                                   screen.y());
        break;
    }

    Character::render(viewport);
    timer.start(get_animation_step(), 100, 1);
}

} // namespace rpg
