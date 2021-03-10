#pragma once

#include "rpg/data/SceneDataReader.h"

namespace rpg::data
{

class SceneInMemoryDataReader : public SceneBaseDataReader
{
  public:
    SceneInMemoryDataReader(const char *scene_name,
                            GlobalCoordinates scene_size)
        : SceneBaseDataReader(scene_name, scene_size)
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

    virtual tilemap_word_t get_image(GlobalCoordinates coordinates,
                                     int layer) override
    {
        coordinates.change_tile_by(tilemap_cell_extra,
                                   tilemap_read_lines_extra);

        unsigned int slice_base = coordinates.tile_y() * tilemap_line_words +
                                  coordinates.tile_x() * tilemap_cell_words;

        unsigned int index = slice_base + layer;
        return scene_data[index];
    }

  private:
    tilemap_word_t *scene_data;

    virtual void read_tilemap(GlobalCoordinates coordinates) override
    {
    }
};

} // namespace rpg::data
