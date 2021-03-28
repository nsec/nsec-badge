#include "rpg/characters/CharacterFarmer.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterFarmer::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterFarmer::render(Viewport &viewport)
{
    if (!is_visible(viewport))
        return;

    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_FARMER_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            if (get_animation_step() % 11 == 1) {
                current_mode = Mode::walking;
                set_moving_direction(get_animation_step() % direction::_count);
                timer.start(get_animation_step(), 111, 33);
            } else {
                switch (get_animation_step() % 2) {
                case 0:
                    current_mode = Mode::walking;
                    set_moving_direction(get_animation_step() %
                                         direction::_count);
                    timer.start(get_animation_step(), 57, 23);
                    break;

                default:
                    current_mode = Mode::standing;
                    timer.start(get_animation_step(), 23, 7);
                }
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
            timer.start(get_animation_step(), 10, 3);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
