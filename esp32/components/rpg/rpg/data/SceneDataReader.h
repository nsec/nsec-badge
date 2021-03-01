#pragma once

#include "rpg_const.h"

#include "graphics.h"

#include "esp_log.h"

#include <array>
#include <climits>
#include <string>

namespace rpg::data
{

using tilemap_word_t = uint8_t;

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
    SceneBaseDataReader(const char *scene_name, int scene_width,
                        int scene_height)
        : scene_width{scene_width}, scene_height{scene_height}, last_x{INT_MIN},
          last_y{INT_MIN}, tilemap_width{scene_width / DISPLAY_TILE_WIDTH +
                                         tilemap_cell_extra},
          tilemap_height{scene_height / DISPLAY_TILE_HEIGHT +
                         tilemap_read_lines_extra}
    {
        std::string filename{"/spiffs/rpg/"};
        filename += scene_name;
        filename += ".scene";

        file = fopen(filename.c_str(), "r");
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

    tilemap_word_t get_dependency(int x, int y)
    {
        return get_image(x, y, 9);
    }

    tilemap_word_t get_flags(int x, int y)
    {
        return get_image(x, y, 8);
    }

    virtual tilemap_word_t get_image(int x, int y, int layer) = 0;

    virtual void read_tilemap(int read_x, int read_y) = 0;

  protected:
    FILE *file;

    /**
     * The total width of the scene in pixels.
     */
    int scene_width;

    /**
     * The total height of the scene in pixels.
     */
    int scene_height;

    /**
     * Coordinates used for the last read operation.
     */
    int last_x;
    int last_y;

    /**
     * The number of tile entries in one row of the tilemap.
     */
    int tilemap_width;

    /**
     * The number of tile rows in the tilemap.
     */
    int tilemap_height;
};

class SceneDataReader : public SceneBaseDataReader
{
  public:
    SceneDataReader(const char *scene_name, int scene_width, int scene_height)
        : SceneBaseDataReader(scene_name, scene_width, scene_height)
    {
        tilemap_slice = new tilemap_slice_t();
        if (!tilemap_slice) {
            ESP_LOGE(__FUNCTION__, "Failed to allocate tilemap_slice.");
            abort();
        }
    }

    ~SceneDataReader()
    {
        delete[] tilemap_slice;
    }

    virtual tilemap_word_t get_image(int x, int y, int layer) override;

    virtual void read_tilemap(int read_x, int read_y) override;

  private:
    tilemap_slice_t *tilemap_slice;
};

} // namespace rpg::data
