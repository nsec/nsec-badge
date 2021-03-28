#include "rpg/characters/CharacterSailor.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterSailor::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterSailor::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_SAILOR_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 2) {
            case 0:
                current_mode = Mode::walking;
                timer.start(get_animation_step(), 31, 7);

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
            render_animation_variant(viewport, Appearance::moving_left, 5);
            break;

        case direction::right:
            render_animation_variant(viewport, Appearance::moving_right, 5);
            break;

        case direction::up:
            render_animation_variant(viewport, Appearance::moving_up, 7);
            break;

        case direction::down:
        default:
            render_animation_variant(viewport, Appearance::moving_down, 7);
        }

        if (timer.has_expired(get_animation_step())) {
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 60, 21);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
