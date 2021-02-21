#include "rpg_data.h"

namespace rpg
{

void SceneDataReader::read_tilemap(int read_x, int read_y)
{
    tilemap_word_t *data = tilemap_slice->data();
    unsigned int offset = read_y * (tilemap_width * tilemap_cell_words) +
                          read_x * tilemap_cell_words;

    for (int i = 0; i < tilemap_read_lines; ++i) {
        fseek(file, offset, SEEK_SET);
        fread(data, sizeof(tilemap_word_t), tilemap_line_words, file);

        data += tilemap_line_words;
        offset += sizeof(tilemap_word_t) * (tilemap_width * tilemap_cell_words);
    }
}

} // namespace rpg
