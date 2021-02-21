#include "rpg/data/BlockedDataReader.h"

namespace rpg::data
{

bool BlockedDataReader::is_blocked(int x, int y)
{
    unsigned int offset = (y / 6 * line_words) + (x / (2 * DISPLAY_TILE_WIDTH));
    unsigned int shift = (x % (2 * DISPLAY_TILE_WIDTH)) / 6;

    return (blocked_data[offset] & (1 << shift)) != 0;
}

} // namespace rpg::data
