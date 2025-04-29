// SPDX-FileCopyrightText: 2025 NorthSec
//
// SPDX-License-Identifier: MIT

#include "badge_ssd1306_helper.hpp"
#include "utils/board.hpp"

static SSD1306_t* dev = nullptr;

void i2c_bus_reset() {
    gpio_config_t conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_4) | (1ULL << GPIO_NUM_5),
        .mode = GPIO_MODE_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&conf);

    gpio_set_level(GPIO_NUM_4, 1);
    gpio_set_level(GPIO_NUM_5, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Manually toggle SCL to release stuck devices
    for (int i = 0; i < 9; i++) {
        gpio_set_level(GPIO_NUM_5, 0);
        vTaskDelay(pdMS_TO_TICKS(5));
        gpio_set_level(GPIO_NUM_5, 1);
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Send STOP condition
    gpio_set_level(GPIO_NUM_4, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(GPIO_NUM_5, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
    gpio_set_level(GPIO_NUM_4, 1);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Return pins to I2C control
    gpio_reset_pin(GPIO_NUM_4);
    gpio_reset_pin(GPIO_NUM_5);
}

// Centrally initialize the ssd1306 to avoid bus conflicts individually
void badge_ssd1306_init()
{
    // Initialize the OLED display
    //esp_log_level_set("SSD1306", ESP_LOG_WARN);
    if (dev == nullptr) {
        i2c_bus_reset();
        dev = new SSD1306_t();
        i2c_master_init(dev, nsec::board::serial::i2c::i2c_sda, nsec::board::serial::i2c::i2c_scl, -1);
        ssd1306_init(dev, 128, 32);
        ssd1306_contrast(dev, 0xff);
    }
    ssd1306_clear_screen(dev, false);
}

// Clear the screen
void badge_ssd1306_clear()
{
    if (dev == nullptr) {
        badge_ssd1306_init();
    }
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

// Deinitialize the SSD1306 device and clean up resources
void badge_ssd1306_deinit()
{
    if (dev != nullptr) {
        // First remove the device from the I2C bus
        if (dev->_i2c_dev_handle != nullptr) {
            i2c_master_bus_rm_device(dev->_i2c_dev_handle);
        }
        
        // Then delete the I2C master bus
        if (dev->_i2c_bus_handle != nullptr) {
            i2c_del_master_bus(dev->_i2c_bus_handle);
        }
        
        // Delete the device and set pointer to null
        delete dev;
        dev = nullptr;
    }
}