#include "rpg/characters/CharacterDog.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterDog::post_render(Viewport &viewport)
{
    moving_mixin();
    Character::post_render(viewport);
}

void CharacterDog::render(Viewport &viewport)
{
    switch (get_moving_direction()) {
    case direction::left:
        render_animation_variant(viewport, Appearance::moving_left, 6);
        break;

    case direction::right:
        render_animation_variant(viewport, Appearance::moving_right, 6);
        break;

    case direction::up:
        render_animation_variant(viewport, Appearance::moving_up, 6);
        break;

    case direction::down:
    default:
        render_animation_variant(viewport, Appearance::moving_down, 6);
    }

    if (timer.has_expired(get_animation_step())) {
        set_moving_direction(get_animation_step() % direction::_count);
        timer.start(get_animation_step(), 97, 31);
    }

    Character::render(viewport);
}

} // namespace rpg
