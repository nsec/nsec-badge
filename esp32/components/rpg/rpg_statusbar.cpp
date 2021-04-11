#include "rpg_statusbar.h"

#include "save.h"

namespace rpg
{

constexpr auto statusbar_position = viewport_crop_height;
constexpr auto statusbar_icons_position = statusbar_position + 4;

void rpg_statusbar_render(Scene *scene)
{
    scene->lock();

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR, 0, statusbar_position);

    for (int i = 0; i < 10; ++i) {
        int icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_OFF;
        int icon_x = i * 14;

        if (i == 0 && Save::save_data.flag1)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 1 && Save::save_data.flag2)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 2 && Save::save_data.flag3)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 3 && Save::save_data.flag4)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 4 && Save::save_data.flag5)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 5 && Save::save_data.flag6)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 6 && Save::save_data.flag7)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 7 && Save::save_data.flag8)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 8 && Save::save_data.flag9)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        if (i == 9 && Save::save_data.flag10)
            icon_image = LIBRARY_IMAGE_STATUS_BAR_FLAG_ON;

        graphics_draw_from_library(icon_image, icon_x,
                                   statusbar_icons_position);
    }

    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_WIFI_OFF, 200,
                               statusbar_icons_position);
    graphics_draw_from_library(LIBRARY_IMAGE_STATUS_BAR_SOUND_ON, 216,
                               statusbar_icons_position);
    graphics_update_display();

    scene->unlock();
}

} // namespace rpg
