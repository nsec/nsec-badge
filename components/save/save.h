#pragma once

#include "esp_err.h"
#include <stdint.h>

struct SaveData {
    uint8_t neopixel_brightness;
    uint8_t neopixel_mode;
    bool neopixel_is_on;
    int neopixel_color;
    bool bluetooth_enabled;
    char wifi_ssid[32];
    char wifi_password[32];
    char encrypted_cells[50];

};

class Save
{
  public:
    static esp_err_t write_save();
    static esp_err_t load_save();
    static SaveData save_data;
};
