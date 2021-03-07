#include "rpg/data/SceneDataReader.h"

namespace rpg::data
{

tilemap_word_t SceneDataReader::get_image(int x, int y, int layer)
{
    unsigned int slice_base =
        (y + tilemap_read_lines_extra) * tilemap_line_words +
        (x + tilemap_cell_extra) * tilemap_cell_words;

    unsigned int index = slice_base + layer;
    return (*tilemap_slice)[index];
}

void SceneDataReader::read_tilemap(GlobalCoordinates coordinates)
{
    if (last_x == coordinates.tile_x() && last_y == coordinates.tile_y())
        return;

    tilemap_word_t *data = tilemap_slice->data();
    unsigned int offset =
        coordinates.tile_y() * (tilemap_width * tilemap_cell_words) +
        coordinates.tile_x() * tilemap_cell_words;

    for (int i = 0; i < tilemap_read_lines; ++i) {
        fseek(file, offset, SEEK_SET);
        fread(data, sizeof(tilemap_word_t), tilemap_line_words, file);

        data += tilemap_line_words;
        offset += sizeof(tilemap_word_t) * (tilemap_width * tilemap_cell_words);
    }

    last_x = coordinates.tile_x();
    last_y = coordinates.tile_y();
}

} // namespace rpg::data
