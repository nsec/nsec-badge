#include "rpg/Character.h"

#include "rpg/Viewport.h"

namespace rpg
{

bool Character::is_visible(Viewport &viewport)
{
    return viewport.is_visible(GlobalCoordinates::xy(scene_x, scene_y), width,
                               height);
}

bool Character::move(GlobalCoordinates coordinates)
{
    if (blocked_data_reader) {
        auto ground = coordinates;
        ground.change_xy_by(get_ground_base_x(), get_ground_base_y());

        if (blocked_data_reader->is_blocked(ground))
            return false;
    }

    move_distance = coordinates;
    move_distance.change_xy_by(-scene_x, -scene_y);

    scene_x = coordinates.x();
    scene_y = coordinates.y();

    return true;
}

void Character::move_initial(GlobalCoordinates coordinates)
{
    move(coordinates);

    // Reset the move_distance that is set by the move() method because it does
    // not make sense for the initial move.
    move_distance = GlobalCoordinates::xy(0, 0);

    initial_scene_x = coordinates.x();
    initial_scene_y = coordinates.y();
}

void Character::render(Viewport &viewport)
{
    rendered_scene_x = scene_x;
    rendered_scene_y = scene_y;
}

void Character::render_animation_variant(Viewport &viewport,
                                         const Appearance appearance,
                                         const int slow_down)
{
    const int *sprites;
    int count;

    if (appearance == Appearance::LENGTH)
        return;

    sprites = animation_variants[static_cast<int>(appearance)].sprites;
    count = animation_variants[static_cast<int>(appearance)].count;

    if (sprites == nullptr || count < 1)
        return;

    int index = (get_animation_step() / slow_down) % count;
    auto screen = viewport.to_screen(get_coordinates());

    graphics_draw_from_library(sprites[index], screen.x(), screen.y());
}

void Character::set_animation_variant(const Appearance appearance,
                                      const int *sprites, const int count)
{
    switch (appearance) {
    case Appearance::LENGTH:
        break;

    default:
        animation_variants[static_cast<int>(appearance)].sprites = sprites;
        animation_variants[static_cast<int>(appearance)].count = count;
    }
}

} // namespace rpg
