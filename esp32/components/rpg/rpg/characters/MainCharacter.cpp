#include "rpg/characters/MainCharacter.h"
#include "rpg/Viewport.h"

#include "graphics.h"
#include "palette.h"

namespace rpg
{

bool MainCharacter::move(GlobalCoordinates coordinates)
{
    if (!Character::move(coordinates))
        return false;

    struct timeval now;
    gettimeofday(&now, NULL);
    move_time = (int64_t)now.tv_sec * 1000000L + (int64_t)now.tv_usec;

    return true;
}

void MainCharacter::render(Viewport &viewport)
{
    struct timeval now;
    gettimeofday(&now, NULL);

    unsigned int time_diff =
        ((int64_t)now.tv_sec * 1000000L + (int64_t)now.tv_usec) - move_time;

    if (time_diff > 4000000) {
        render_animation_variant(viewport, Appearance::standing, 2);
    } else if (time_diff > 150000) {
        int image = 0;
        auto move_distance = get_move_distance();

        if (move_distance.y() < 0) {
            image = LIBRARY_IMAGE_MC_12;
        } else if (move_distance.y() > 0) {
            image = LIBRARY_IMAGE_MC_00;
        } else if (move_distance.x() < 0) {
            image = LIBRARY_IMAGE_MC_16;
        } else if (move_distance.x() > 0) {
            image = LIBRARY_IMAGE_MC_24;
        }

        auto screen = viewport.to_screen(get_coordinates());
        graphics_draw_from_library(image, screen.x(), screen.y());
    } else {
        auto move_distance = get_move_distance();

        if (move_distance.y() < 0) {
            render_animation_variant(viewport, Appearance::moving_up);
        } else if (move_distance.y() > 0) {
            render_animation_variant(viewport, Appearance::moving_down);
        } else if (move_distance.x() < 0) {
            render_animation_variant(viewport, Appearance::moving_left);
        } else if (move_distance.x() > 0) {
            render_animation_variant(viewport, Appearance::moving_right);
        } else {
            render_animation_variant(viewport, Appearance::standing, 2);
        }
    }

    Character::render(viewport);
}

} // namespace rpg
