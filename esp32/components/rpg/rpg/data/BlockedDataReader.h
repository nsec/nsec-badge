#pragma once

#include "rpg/Coordinates.h"
#include "rpg_const.h"

#include "graphics.h"

#include "esp_log.h"

#include <string>

namespace rpg::data
{

class BlockedDataReader
{
  public:
    BlockedDataReader(const char *scene_name, GlobalCoordinates scene_size)
        : scene_size(scene_size)
    {
        std::string filename{"/spiffs/rpg/"};
        filename += scene_name;
        filename += ".blocked";

        file = fopen(filename.c_str(), "r+");
        if (!file) {
            ESP_LOGE(__FUNCTION__,
                     "Scene blocked area cannot be read from SPIFFS: %s",
                     filename.c_str());
            abort();
        }

        line_words = scene_size.x() / (2 * DISPLAY_TILE_WIDTH);
        read_size = line_words * (scene_size.y() / 6);

        blocked_data =
            static_cast<uint8_t *>(calloc(read_size, sizeof(uint8_t)));

        if (!blocked_data) {
            ESP_LOGE(__FUNCTION__, "Failed to allocate blocked_data.");
            abort();
        }

        refresh();
    }

    ~BlockedDataReader()
    {
        delete[] blocked_data;

        if (file)
            fclose(file);
    }

    bool is_blocked(GlobalCoordinates coordinates);

    friend class rpg::KonamiHandler;

  private:
    FILE *file;

    uint8_t *blocked_data;

    const GlobalCoordinates scene_size;

    unsigned int line_words = 0;
    int read_size = 0;

    void patch(unsigned int offset, uint8_t value);

    void refresh();
};

} // namespace rpg::data
