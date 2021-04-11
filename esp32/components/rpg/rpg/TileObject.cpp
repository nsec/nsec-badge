#include "rpg/TileObject.h"

#include "rpg/Coordinates.h"

namespace rpg
{

TileObject::~TileObject()
{
}

void TileObject::render(Viewport &viewport)
{
    if (!viewport.is_visible(coordinates, width, height))
        return;

    auto screen = viewport.to_screen(coordinates);
    graphics_draw_from_library(tile_image, screen.x(), screen.y());
}

} // namespace rpg
