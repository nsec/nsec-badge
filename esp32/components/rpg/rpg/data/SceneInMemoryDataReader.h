#pragma once

#include "rpg/data/SceneDataReader.h"

namespace rpg::data
{

class SceneInMemoryDataReader : public SceneBaseDataReader
{
  public:
    SceneInMemoryDataReader(const char *scene_name, int scene_width,
                            int scene_height)
        : SceneBaseDataReader(scene_name, scene_width, scene_height)
    {
        size_t scene_data_read_size =
            tilemap_width * tilemap_height * tilemap_cell_words;

        scene_data = static_cast<tilemap_word_t *>(
            calloc(scene_data_read_size, sizeof(tilemap_word_t)));
        if (!scene_data) {
            ESP_LOGE(__FUNCTION__, "Failed to allocate scene_data.");
            abort();
        }

        fseek(file, 0, SEEK_SET);
        fread(scene_data, sizeof(tilemap_word_t), scene_data_read_size, file);
    }

    ~SceneInMemoryDataReader()
    {
        delete[] scene_data;
    }

    virtual tilemap_word_t get_image(int x, int y, int layer) override
    {
        unsigned int slice_base =
            ((last_y + y + tilemap_read_lines_extra) *
             (tilemap_width * tilemap_cell_words)) +
            ((last_x + x + tilemap_cell_extra) * tilemap_cell_words);

        unsigned int index = slice_base + layer;
        return scene_data[index];
    }

    virtual void read_tilemap(int read_x, int read_y) override
    {
        last_x = read_x;
        last_y = read_y;
    }

  private:
    tilemap_word_t *scene_data;
};

} // namespace rpg::data
