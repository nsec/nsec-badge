#include "rpg/characters/CharacterVijay.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterVijay::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterVijay::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_VIJAY_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            if (get_animation_step() % 7 == 1) {
                current_mode = Mode::walking;
                set_moving_direction((get_animation_step() + 3) %
                                     direction::_count);
                timer.start(get_animation_step(), 171, 59);
            } else {
                switch (get_animation_step() % 2) {
                case 0:
                    current_mode = Mode::walking;
                    set_moving_direction((get_animation_step() + 1) %
                                         direction::_count);
                    timer.start(get_animation_step(), 23, 7);
                    break;

                default:
                    current_mode = Mode::standing;
                    timer.start(get_animation_step(), 17, 7);
                }
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
            render_animation_variant(viewport, Appearance::moving_up, 5);
            break;

        case direction::down:
        default:
            render_animation_variant(viewport, Appearance::moving_down, 5);
        }

        if (timer.has_expired(get_animation_step())) {
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 19, 17);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
