#include "rpg/TileObject.h"

#include "rpg/Coordinates.h"

namespace rpg
{

TileObject::~TileObject()
{
}

void TileObject::render(Viewport &viewport)
{
    auto c = coordinates;

    if (!viewport.is_visible(c)) {
        c.change_xy_by(width, 0);

        if (!viewport.is_visible(c)) {
            c.change_xy_by(0, height);

            if (!viewport.is_visible(c)) {
                c.change_xy_by(-width, 0);

                if (!viewport.is_visible(c))
                    return;
            }
        }
    }

    auto screen = viewport.to_screen(coordinates);
    graphics_draw_from_library(tile_image, screen.x(), screen.y());
}

} // namespace rpg
