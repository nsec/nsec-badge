#include "rpg/data/BlockedDataReader.h"

namespace rpg::data
{

bool BlockedDataReader::is_blocked(GlobalCoordinates coordinates)
{
    if (coordinates.x() < 0 || coordinates.y() < 0)
        return true;

    if (coordinates.x() >= scene_size.x() || coordinates.y() > scene_size.y())
        return true;

    unsigned int offset = ((coordinates.y() / 6) * line_words) +
                          (coordinates.x() / (2 * DISPLAY_TILE_WIDTH));

    unsigned int shift = (coordinates.x() % (2 * DISPLAY_TILE_WIDTH)) / 6;

    return (blocked_data[offset] & (1 << shift)) != 0;
}

void BlockedDataReader::patch(unsigned int offset, uint8_t value)
{
    fseek(file, offset, SEEK_SET);
    fwrite(&value, sizeof(value), 1, file);
}

void BlockedDataReader::refresh()
{
    fseek(file, 0, SEEK_SET);
    fread(blocked_data, sizeof(uint8_t), read_size, file);
}

} // namespace rpg::data
