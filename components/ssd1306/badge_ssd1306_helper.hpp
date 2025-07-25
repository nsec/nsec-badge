#ifndef BADGE_SSD1306_HELPER_H_
#define BADGE_SSD1306_HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_console.h"
#include "ssd1306.h"
#include "driver/i2c.h"
#include "linenoise/linenoise.h"
#include "cmd_sys.h"

void badge_ssd1306_init();
void badge_ssd1306_clear();
void badge_print_bitmap(uint8_t* bitmap, int x, int y, int width, int height, bool invert);
void badge_print_text(int line, const char* text, int size, bool invert);
void badge_print_textbox(int line, int seg, char* text, int boxsize, int textsize, bool invert, int speed);
void badge_ssd1306_deinit();
#endif
