#include "rpg/characters/CharacterDancer.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterDancer::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterDancer::render(Viewport &viewport)
{
    if (!is_visible(viewport))
        return;

    switch (current_mode) {
    case Mode::dancing:
        render_dancing(viewport);
        break;

    case Mode::walking:
        render_walking(viewport);
        break;

    case Mode::standing:
    default:
        render_standing(viewport);
    }

    Character::render(viewport);
}

void CharacterDancer::render_dancing(Viewport &viewport)
{
    render_animation_variant(viewport, Appearance::action_a, 3);

    if (timer.has_expired(get_animation_step())) {
        switch (get_animation_step() % 8) {
        case 0:
            current_mode = Mode::dancing;
            timer.start(get_animation_step(), 23, 7);
            break;

        case 1:
        case 2:
        case 3:
        case 4:
            current_mode = Mode::walking;
            set_moving_direction(get_animation_step() % direction::_count);
            timer.start(get_animation_step(), 61, 23);
            break;

        default:
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 11, 3);
        }
    }
}

void CharacterDancer::render_standing(Viewport &viewport)
{
    auto coordinates = viewport.to_screen(get_coordinates());
    graphics_draw_from_library(LIBRARY_IMAGE_DANCER_STANDING, coordinates.x(),
                               coordinates.y());

    if (timer.has_expired(get_animation_step())) {
        switch (get_animation_step() % 8) {
        case 0:
        case 1:
        case 2:
            current_mode = Mode::dancing;
            timer.start(get_animation_step(), 30, 15);
            break;

        case 3:
        case 4:
        case 5:
            current_mode = Mode::walking;
            set_moving_direction(get_animation_step() % direction::_count);
            timer.start(get_animation_step(), 90, 50);
            break;

        default:
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 17, 5);
        }
    }
}

void CharacterDancer::render_walking(Viewport &viewport)
{
    switch (get_moving_direction()) {
    case direction::left:
        render_animation_variant(viewport, Appearance::moving_left, 2);
        break;

    case direction::right:
        render_animation_variant(viewport, Appearance::moving_right, 2);
        break;

    case direction::up:
        render_animation_variant(viewport, Appearance::moving_up, 4);
        break;

    case direction::down:
    default:
        render_animation_variant(viewport, Appearance::moving_down, 4);
    }

    if (timer.has_expired(get_animation_step())) {
        switch (get_animation_step() % 7) {
        case 0:
        case 1:
        case 2:
        case 3:
            current_mode = Mode::walking;
            set_moving_direction(get_animation_step() % direction::_count);
            timer.start(get_animation_step(), 13, 5);
            break;

        default:
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 7, 6);
        }
    }
}

} // namespace rpg
