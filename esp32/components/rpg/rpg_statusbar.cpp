#include "rpg_statusbar.h"

namespace rpg
{

constexpr auto statusbar_position = viewport_crop_height;
constexpr auto statusbar_icons_position = statusbar_position + 4;

void rpg_statusbar_render(Scene *scene)
{
    scene->lock();

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR, 0, statusbar_position);

    for (int i = 0; i < 10; ++i) {
        if (i % 3 == 0)
            graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_FLAG_ON, i * 14,
                                       statusbar_icons_position);
        else
            graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_FLAG_OFF,
                                       i * 14, statusbar_icons_position);
    }

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_WIFI_OFF, 200,
                               statusbar_icons_position);
    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_SOUND_ON, 216,
                               statusbar_icons_position);
    graphics_update_display();

    scene->unlock();
}

} // namespace rpg
