#pragma once

#include "esp_err.h"
#include <stdint.h>

struct SaveData {
    uint8_t neopixel_brightness;
    uint8_t neopixel_mode;
    bool neopixel_is_on;
    int neopixel_color;
    bool flag1;
    bool flag2;
    bool flag3;
    bool flag4;
    bool flag5;
    bool flag6;
    bool flag7;
    bool flag8;
    bool flag9;
    bool flag10;
    bool flag11;
    bool flag12;
    bool flag13;
    bool flag14;
    bool flag15;
};

class Save
{
  public:
    static esp_err_t write_save();
    static esp_err_t load_save();
    static SaveData save_data;
};
