#include "rpg/ChestObject.h"

#include "rpg/Viewport.h"

namespace rpg
{

void ChestObject::render(Viewport &viewport)
{
    if (!viewport.is_visible(coordinates, get_width(), get_height()))
        return;

    auto screen = viewport.to_screen(coordinates);

    if (is_opened())
        graphics_draw_from_library(LIBRARY_IMAGE_CHEST_2, screen.x(),
                                   screen.y());
    else
        graphics_draw_from_library(LIBRARY_IMAGE_CHEST_1, screen.x(),
                                   screen.y());
}

} // namespace rpg
