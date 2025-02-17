#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_console.h"
#include "ssd1306.h"
#include "driver/i2c.h"
#include "linenoise/linenoise.h"
#include "cmd_sys.h"

//static const char *TAG = "badge_ssd1306_helper";

void badge_ssd1306_init();
void badge_ssd1306_clear();
void badge_print_bitmap(uint8_t* bitmap, int x, int y, int width, int height, bool invert);
void badge_print_text(int line, char* text, int size, bool invert);
void badge_print_textbox(int line, int seg, char* text, int boxsize, int textsize, bool invert, int speed);
