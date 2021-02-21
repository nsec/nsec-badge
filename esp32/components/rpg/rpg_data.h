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

constexpr unsigned int tilemap_entry_words = 10;
constexpr unsigned int tilemap_entry_length =
    sizeof(tilemap_word_t) * tilemap_entry_words;

using tilemap_t = std::array<tilemap_word_t, tilemap_entry_length>;

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

        tilemap_width = scene_width / DISPLAY_TILE_WIDTH;
    }

    ~SceneDataReader()
    {
        if (file) {
            fclose(file);
        }
    }

    void read_tilemap(int tile_x, int tile_y, tilemap_t &tilemap);

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
};

} // namespace rpg
