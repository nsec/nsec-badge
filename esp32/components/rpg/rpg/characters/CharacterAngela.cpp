#include "rpg/characters/CharacterAngela.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterAngela::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterAngela::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        render_standing(viewport);
    } else if (current_mode == Mode::walking) {
        render_walking(viewport);
    }

    Character::render(viewport);
}

void CharacterAngela::render_standing(Viewport &viewport)
{
    auto screen = viewport.to_screen(get_coordinates());
    graphics_draw_from_library(LIBRARY_IMAGE_ANGELA_STANDING, screen.x(),
                               screen.y());

    if (timer.has_expired(get_animation_step())) {
        if (get_animation_step() % 9 == 1) {
            set_moving_direction(direction::left);
            timer.start(get_animation_step(), 119, 31);
            return;
        }

        switch (get_animation_step() % 3) {
        case 0:
            current_mode = Mode::walking;
            set_moving_direction(get_animation_step() % direction::_count);
            timer.start(get_animation_step(), 59, 23);
            break;

        default:
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 17, 7);
        }
    }
}

void CharacterAngela::render_walking(Viewport &viewport)
{
    switch (get_moving_direction()) {
    case direction::left:
        render_animation_variant(viewport, Appearance::moving_left, 3);
        break;

    case direction::right:
        render_animation_variant(viewport, Appearance::moving_right, 3);
        break;

    case direction::up:
        render_animation_variant(viewport, Appearance::moving_up, 4);
        break;

    case direction::down:
    default:
        render_animation_variant(viewport, Appearance::moving_down, 4);
    }

    if (timer.has_expired(get_animation_step())) {
        current_mode = Mode::standing;
        timer.start(get_animation_step(), 13, 5);
    }
}

} // namespace rpg
