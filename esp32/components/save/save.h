#pragma once

#include "esp_err.h"
#include <stdint.h>

struct SaveData {
    bool buzzer_enable_music;
    int buzzer_enable_music_id;
    bool buzzer_enable_sfx;
    bool buzzer_enable_steps;
    bool chest_opened_island;
    bool chest_opened_konami;
    bool chest_opened_welcome;
    bool konami_done;
    uint8_t neopixel_brightness;
    uint8_t neopixel_mode;
    bool neopixel_is_on;
    int neopixel_color;

    // Welcome flag.
    bool flag1;

    // Konami code flag.
    bool flag2;

    // Island flag.
    bool flag3;

    // Duck flag.
    bool flag4;

    // Punk dialog flag.
    bool flag5;

    // Wifi connection flag.
    bool flag6;

    // Hidden CLI command flag.
    bool flag7;

    // Reverse 101 flag.
    bool flag8;

    // Reverse 102 flag.
    bool flag9;

    // Unused.
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
