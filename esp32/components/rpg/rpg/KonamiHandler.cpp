#include "rpg/KonamiHandler.h"

#include "rpg/Coordinates.h"

#include "save.h"

namespace rpg
{

void KonamiHandler::patch()
{
    if (Save::save_data.konami_done)
        return;

    auto coordinates =
        scene.get_viewport().to_screen(GlobalCoordinates::tile(0, 8));
    if (coordinates.x() < 0 || coordinates.y() < 0) {
        // TODO play "bad" sound.
        return;
    }

    scene.data_reader.patch(0x2cf2, LIBRARY_IMAGE_OUTER_WALL_E);
    scene.data_reader.patch(0x2cf8, LIBRARY_IMAGE_OUTER_WALL_E_ARCH);
    scene.data_reader.refresh();

    blocked_data_reader.patch(0x36b, 0xbf);
    blocked_data_reader.patch(0x384, 0xbf);
    blocked_data_reader.patch(0x39d, 0xbf);
    blocked_data_reader.patch(0x3b6, 0xbf);
    blocked_data_reader.patch(0x3cf, 0xbf);
    blocked_data_reader.patch(0x3e8, 0xbf);
    blocked_data_reader.patch(0x401, 0xbf);
    blocked_data_reader.refresh();

    Save::save_data.konami_done = true;
    Save::write_save();

    // TODO play "good" sound.

    scene.get_viewport().mark_for_full_refresh();
}

} // namespace rpg
