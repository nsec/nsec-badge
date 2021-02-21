#pragma once

extern "C" {
#include "graphics.h"
}

#include "esp_log.h"

#include <array>
#include <string>

namespace rpg
{

using tilemap_word_t = uint8_t;

constexpr unsigned int tilemap_cell_extra = 2;
constexpr unsigned int tilemap_cell_words = 10;
constexpr unsigned int tilemap_line_words =
    tilemap_cell_words * (DISPLAY_TILES_X + tilemap_cell_extra);

constexpr unsigned int tilemap_read_lines = 11;
constexpr unsigned int tilemap_read_lines_extra = 2;

using tilemap_slice_t =
    std::array<tilemap_word_t, (tilemap_line_words * tilemap_read_lines)>;

class SceneDataReader
{

  public:
    SceneDataReader(const char *scene_name, int scene_width)
        : scene_width{scene_width}
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

        tilemap_slice = new tilemap_slice_t();
        if (!tilemap_slice) {
            ESP_LOGE(__FUNCTION__, "Failed to allocate tilemap_slice.");
            abort();
        }

        tilemap_width = scene_width / DISPLAY_TILE_WIDTH + tilemap_cell_extra;
    }

    ~SceneDataReader()
    {
        delete tilemap_slice;

        if (file) {
            fclose(file);
        }
    }

    tilemap_word_t get_dependency(int x, int y)
    {
        return get_image(x, y, 9);
    }

    tilemap_word_t get_flags(int x, int y)
    {
        return get_image(x, y, 8);
    }

    tilemap_word_t get_image(int x, int y, int layer)
    {
        unsigned int index = calculate_slice_base(x, y) + layer;
        return (*tilemap_slice)[index];
    }

    void read_tilemap(int read_x, int read_y);

  private:
    FILE *file;

    /**
     * The total width of the scene in pixels.
     */
    int scene_width;

    /**
     * The number of tile entries in one row of the tilemap.
     *
     * This value is only used to calculate the correct read offset.
     *
     * Note that the tilemap_height field does not exist: the tile coordinates
     * are not validated and it is entirely responsbility of the class user to
     * prevent out-of-bound reads.
     */
    int tilemap_width;

    tilemap_slice_t *tilemap_slice;

    unsigned int calculate_slice_base(int x, int y)
    {
        return (y + tilemap_read_lines_extra) * tilemap_line_words +
               (x + tilemap_cell_extra) * tilemap_cell_words;
    }
};

} // namespace rpg
