#include "rpg/data/BlockedDataReader.h"

namespace rpg::data
{

bool BlockedDataReader::is_blocked(GlobalCoordinates coordinates)
{
    unsigned int offset = ((coordinates.y() / 6) * line_words) +
                          (coordinates.x() / (2 * DISPLAY_TILE_WIDTH));

    unsigned int shift = (coordinates.x() % (2 * DISPLAY_TILE_WIDTH)) / 6;

    return (blocked_data[offset] & (1 << shift)) != 0;
}

} // namespace rpg::data
