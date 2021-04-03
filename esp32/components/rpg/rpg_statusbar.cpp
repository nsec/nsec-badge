#include "rpg_statusbar.h"

namespace rpg
{

constexpr auto statusbar_position = viewport_crop_height;

void rpg_statusbar_render(Scene *scene)
{
    scene->lock();

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR, 0, statusbar_position);
    graphics_update_display();

    scene->unlock();
}

} // namespace rpg
