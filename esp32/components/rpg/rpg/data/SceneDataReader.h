#pragma once

#include "rpg/Coordinates.h"
#include "rpg_const.h"

#include "graphics.h"

#include "esp_log.h"

#include <array>
#include <climits>
#include <string>

namespace rpg::data
{

using tilemap_word_t = uint16_t;

using tilemap_slice_t =
    std::array<tilemap_word_t, (tilemap_line_words * tilemap_read_lines)>;

struct tilemap_dependency_t {
    char backward_x;
    char backward_y;
    char forward_x;
    char forward_y;
};

class SceneBaseDataReader
{
  public:
    SceneBaseDataReader(const char *scene_name, GlobalCoordinates scene_size)
        : scene_size{scene_size}, tilemap_width{scene_size.tile_x() +
                                                tilemap_cell_extra},
          tilemap_height{scene_size.y() + tilemap_read_lines_extra}
    {
        std::string filename{"/spiffs/rpg/"};
        filename += scene_name;
        filename += ".scene";

        file = fopen(filename.c_str(), "r+");
        if (!file) {
            ESP_LOGE(__FUNCTION__,
                     "Scene datafile cannot be read from SPIFFS: %s",
                     filename.c_str());
            abort();
        }
    }

    ~SceneBaseDataReader()
    {
        if (file) {
            fclose(file);
        }
    }

    tilemap_dependency_t decode_dependency(tilemap_word_t dependency) const
    {
        return {
            .backward_x = static_cast<char>((dependency >> 6) & 0x03),
            .backward_y = static_cast<char>((dependency >> 4) & 0x03),
            .forward_x = static_cast<char>((dependency >> 2) & 0x03),
            .forward_y = static_cast<char>(dependency & 0x03),
        };
    }

    tilemap_word_t get_dependency(GlobalCoordinates coordinates)
    {
        return get_image(coordinates, 9);
    }

    tilemap_word_t get_flags(GlobalCoordinates coordinates)
    {
        return get_image(coordinates, 8);
    }

    virtual tilemap_word_t get_image(GlobalCoordinates coordinates,
                                     int layer) = 0;

  protected:
    FILE *file;

    /**
     * The size of the scene, expressed as bottom-right corner coordinates.
     */
    GlobalCoordinates scene_size;

    /**
     * Coordinates used for the last read operation.
     */
    GlobalCoordinates current_start = GlobalCoordinates::xy(0, 0);
    GlobalCoordinates current_end = GlobalCoordinates::xy(0, 0);

    /**
     * The number of tile entries in one row of the tilemap.
     */
    int tilemap_width;

    /**
     * The number of tile rows in the tilemap.
     */
    int tilemap_height;

    virtual void patch(unsigned int offset, uint8_t value) = 0;

    virtual void read_tilemap(GlobalCoordinates coordinates) = 0;

    virtual void refresh() = 0;
};

class SceneDataReader : public SceneBaseDataReader
{
  public:
    SceneDataReader(const char *scene_name, GlobalCoordinates scene_size)
        : SceneBaseDataReader(scene_name, scene_size)
    {
        tilemap_slice = new tilemap_slice_t();
        if (!tilemap_slice) {
            ESP_LOGE(__FUNCTION__, "Failed to allocate tilemap_slice.");
            abort();
        }

        refresh();
    }

    ~SceneDataReader()
    {
        delete[] tilemap_slice;
    }

    virtual tilemap_word_t get_image(GlobalCoordinates coordinates,
                                     int layer) override;

  protected:
    virtual void patch(unsigned int offset, uint8_t value) override;

    virtual void refresh();

  private:
    tilemap_slice_t *tilemap_slice;

    virtual void read_tilemap(GlobalCoordinates coordinates) override;
};

} // namespace rpg::data
