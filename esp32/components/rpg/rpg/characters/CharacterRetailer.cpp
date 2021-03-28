#include "rpg/characters/CharacterRetailer.h"

#include "rpg/Viewport.h"

#include "graphics.h"

namespace rpg
{

void CharacterRetailer::post_render(Viewport &viewport)
{
    if (current_mode == Mode::walking)
        moving_mixin();

    Character::post_render(viewport);
}

void CharacterRetailer::render(Viewport &viewport)
{
    if (current_mode == Mode::standing) {
        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(LIBRARY_IMAGE_RETAILER_STANDING, screen.x(),
                                   screen.y());

        if (timer.has_expired(get_animation_step())) {
            switch (get_animation_step() % 5) {
            case 0:
                current_mode = Mode::walking;
                timer.start(get_animation_step(), 24, 11);

                set_moving_direction(get_animation_step() % 2
                                         ? direction::left
                                         : direction::right);

                break;

            default:
                current_mode = Mode::standing;
                timer.start(get_animation_step(), 18, 6);
            }
        }
    } else if (current_mode == Mode::walking) {
        switch (get_moving_direction()) {
        case direction::left:
            render_animation_variant(viewport, Appearance::moving_left, 4);
            break;

        case direction::right:
        default:
            render_animation_variant(viewport, Appearance::moving_right, 4);
        }

        if (timer.has_expired(get_animation_step())) {
            current_mode = Mode::standing;
            timer.start(get_animation_step(), 8, 3);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
