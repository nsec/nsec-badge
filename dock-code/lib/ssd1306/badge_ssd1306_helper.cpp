// SPDX-FileCopyrightText: 2025 NorthSec
//
// SPDX-License-Identifier: MIT

#include "badge_ssd1306_helper.hpp"
//#include "utils/board.hpp"

static SSD1306_t* dev = nullptr;
#define SDA_PIN 47
#define SCL_PIN 48

// Centrally initialize the ssd1306 to avoid bus conflicts individually
void badge_ssd1306_init()
{
    // Initialize the OLED display
    //esp_log_level_set("SSD1306", ESP_LOG_WARN);
    if (dev == nullptr) {
        dev = new SSD1306_t();
        i2c_master_init(dev, SDA_PIN, SCL_PIN, -1);
        ssd1306_init(dev, 128, 64);
        ssd1306_contrast(dev, 0xff);
    }
    ssd1306_clear_screen(dev, false);
}

// Clear the screen
void badge_ssd1306_clear()
{
    ssd1306_clear_screen(dev, false);
}

// Print bitmap on the screen - max dimensions 128x32 total
void badge_print_bitmap(uint8_t* bitmap, int x, int y, int width, int height, bool invert)
{
    // Check if initialized
    if (dev == nullptr) {
        badge_ssd1306_init();
    }
    ssd1306_bitmaps(dev, x, y, bitmap, width, height, invert);
}

// Print text to specific line to ssd1306 screen
// Line is between 0-3, 0 being the top line
// size is len(text) - one line being max 16 chars (see textbox func for longer text)
// Invert being false is white text on black background, true is black text on white background 
void badge_print_text(int line, char* text, int size, bool invert)
{
    // Check if initialized
    if (dev == nullptr) {
        badge_ssd1306_init();
    }
    ssd1306_display_text(dev, line, text, size, invert);
}

// Print text to specific line to ssd1306 screen
// ssd1306_display_text_box1(dev, 3,  0, const_cast<char*>(flag.c_str()), 16, 16, false, 100);
// Line is between 0-3, 0 being the top line
// boxsize is the size of the box to display the text in - can be up to max 16 characters
// textsize is len(text) - can be more than 16 characters
// Invert being false is white text on black background, true is black text on white background
// speed is the speed of the text scrolling
void badge_print_textbox(int line, int seg, char* text, int boxsize, int textsize, bool invert, int speed)
{
// Check if initialized
    if (dev == nullptr) {
        badge_ssd1306_init();
    }
    ssd1306_display_text_box1(dev, line, seg, text, boxsize, textsize, invert, speed);
}