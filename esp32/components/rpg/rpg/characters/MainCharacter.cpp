#include "rpg/characters/MainCharacter.h"
#include "rpg/Viewport.h"

#include "graphics.h"
#include "palette.h"

namespace rpg
{

void MainCharacter::move(GlobalCoordinates coordinates)
{
    auto ground = coordinates;
    ground.change_xy_by(get_ground_base_x(), get_ground_base_y());

    if (data_reader.is_blocked(ground))
        return;

    move_dx = coordinates.x() - get_coordinates().x();
    move_dy = coordinates.y() - get_coordinates().y();

    if (move_dx != 0 || move_dy != 0) {
        struct timeval now;
        gettimeofday(&now, NULL);
        move_time = (int64_t)now.tv_sec * 1000000L + (int64_t)now.tv_usec;
    }

    Character::move(coordinates);
}

void MainCharacter::render(Viewport &viewport)
{
    int image = LIBRARY_IMAGE_MC_00;

    struct timeval now;
    gettimeofday(&now, NULL);

    unsigned int time_diff =
        ((int64_t)now.tv_sec * 1000000L + (int64_t)now.tv_usec) - move_time;

    if (time_diff > 250000) {
        switch ((get_animation_step() / 2) % 2) {
        case 1:
            image = LIBRARY_IMAGE_MC_04;
            break;
        default:
            image = LIBRARY_IMAGE_MC_00;
        }
    } else {
        if (move_dy != 0) {
            switch (get_animation_step() % 6) {
            case 0:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_08 : LIBRARY_IMAGE_MC_01;
                break;
            case 1:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_09 : LIBRARY_IMAGE_MC_02;
                break;
            case 2:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_10 : LIBRARY_IMAGE_MC_03;
                break;
            case 3:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_12 : LIBRARY_IMAGE_MC_05;
                break;
            case 4:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_13 : LIBRARY_IMAGE_MC_06;
                break;
            case 5:
            default:
                image = move_dy < 0 ? LIBRARY_IMAGE_MC_14 : LIBRARY_IMAGE_MC_07;
                break;
            }
        } else {
            switch (get_animation_step() % 6) {
            case 0:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_17 : LIBRARY_IMAGE_MC_25;
                break;
            case 1:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_18 : LIBRARY_IMAGE_MC_26;
                break;
            case 2:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_19 : LIBRARY_IMAGE_MC_27;
                break;
            case 3:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_21 : LIBRARY_IMAGE_MC_29;
                break;
            case 4:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_22 : LIBRARY_IMAGE_MC_30;
                break;
            case 5:
            default:
                image = move_dx < 0 ? LIBRARY_IMAGE_MC_23 : LIBRARY_IMAGE_MC_31;
                break;
            }
        }
    }

    auto coordinates = viewport.to_screen(get_coordinates());
    graphics_draw_from_library(image, coordinates.x(), coordinates.y());
    Character::render(viewport);
}

} // namespace rpg
