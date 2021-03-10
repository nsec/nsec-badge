#include "rpg/data/SceneDataReader.h"

namespace rpg::data
{

tilemap_word_t SceneDataReader::get_image(GlobalCoordinates coordinates,
                                          int layer)
{
    coordinates.change_tile_by(tilemap_cell_extra, tilemap_read_lines_extra);

    if (!coordinates.within(current_start, current_end))
        read_tilemap(coordinates);

    coordinates.change_tile_by(-current_start.tile_x(),
                               -current_start.tile_y());

    unsigned int slice_base = coordinates.tile_y() * tilemap_line_words +
                              coordinates.tile_x() * tilemap_cell_words;

    unsigned int index = slice_base + layer;
    return (*tilemap_slice)[index];
}

void SceneDataReader::read_tilemap(GlobalCoordinates coordinates)
{
    current_start = coordinates;

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

    coordinates.change_tile_by(viewport_tiles_width + 1,
                               viewport_tiles_height + 1);
    current_end = coordinates;
}

} // namespace rpg::data
