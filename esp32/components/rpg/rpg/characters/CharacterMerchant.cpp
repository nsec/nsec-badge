#include "rpg/characters/CharacterMerchant.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterMerchant::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterMerchant::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_MERCHANT_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 2) {
            case 0:
                current_mode = Mode::walking;
                timer.start(get_animation_step(), 37, 11);

                switch (get_animation_step() % 8) {
                case 0:
                case 1:
                case 2:
                    set_moving_direction(direction::left);
                    break;

                case 3:
                case 4:
                case 5:
                    set_moving_direction(direction::right);
                    break;

                case 6:
                    set_moving_direction(direction::up);
                    break;

                case 7:
                default:
                    set_moving_direction(direction::down);
                    break;
                }

                break;

            default:
                current_mode = Mode::standing;
                timer.start(get_animation_step(), 41, 23);
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
            render_animation_variant(viewport, Appearance::moving_up, 5);
            break;

        case direction::down:
        default:
            render_animation_variant(viewport, Appearance::moving_down, 5);
        }

        if (timer.has_expired(get_animation_step())) {
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 17, 9);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
