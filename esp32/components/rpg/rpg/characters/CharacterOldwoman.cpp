#include "rpg/characters/CharacterOldwoman.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterOldwoman::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterOldwoman::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_OLDWOMAN_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 2) {
            case 0:
                current_mode = Mode::walking;
                set_moving_direction(get_animation_step() % direction::_count);
                timer.start(get_animation_step(), 37, 11);
                break;

            default:
                current_mode = Mode::standing;
                timer.start(get_animation_step(), 21, 11);
            }
        }
    } else if (current_mode == Mode::walking) {
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
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 6, 1);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
