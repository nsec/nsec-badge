#include "rpg/characters/CharacterAristocrate.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterAristocrate::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterAristocrate::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_ARISTO_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 3) {
            case 0:
                current_mode = Mode::walking;
                set_moving_direction(get_animation_step() % direction::_count);
                timer.start(get_animation_step(), 57, 23);
                break;

            default:
                current_mode = Mode::standing;
                timer.start(get_animation_step(), 57, 7);
            }
        }
    } else if (current_mode == Mode::walking) {
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
            timer.start(get_animation_step(), 10, 3);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
