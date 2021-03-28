#include "rpg/characters/CharacterRed.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterRed::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterRed::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_RED_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 3) {
            case 0:
                current_mode = Mode::walking;
                timer.start(get_animation_step(), 39, 11);

                switch (get_animation_step() % 8) {
                case 0:
                    set_moving_direction(direction::left);
                    break;

                case 1:
                    set_moving_direction(direction::right);
                    break;

                case 2:
                case 3:
                case 4:
                    set_moving_direction(direction::up);
                    break;

                case 5:
                case 6:
                case 7:
                default:
                    set_moving_direction(direction::down);
                    break;
                }

                break;

            default:
                current_mode = Mode::standing;
                timer.start(get_animation_step(), 40, 21);
            }
        }
    } else if (current_mode == Mode::walking) {
        switch (get_moving_direction()) {
        case direction::left:
            render_animation_variant(viewport, Appearance::moving_left, 4);
            break;

        case direction::right:
            render_animation_variant(viewport, Appearance::moving_right, 4);
            break;

        case direction::up:
            render_animation_variant(viewport, Appearance::moving_up, 6);
            break;

        case direction::down:
        default:
            render_animation_variant(viewport, Appearance::moving_down, 6);
        }

        if (timer.has_expired(get_animation_step())) {
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 21, 13);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
