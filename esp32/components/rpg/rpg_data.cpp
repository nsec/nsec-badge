#include "rpg_data.h"

namespace rpg
{

void SceneDataReader::read_tilemap(int tile_x, int tile_y, tilemap_t &tilemap)
{
    int offset = ((tile_y * tilemap_width) + tile_x) * tilemap_entry_length;
    tilemap_word_t *data = tilemap.data();

    fseek(file, offset, SEEK_SET);
    fread(data, sizeof(tilemap_word_t), tilemap_entry_words, file);
}

} // namespace rpg
