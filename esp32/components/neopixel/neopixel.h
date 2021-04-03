#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void enable_neopixels();
void neopixel_init();
void neopixel_stop();
void neopixel_set_brightness(uint8_t brightness);
void neopixel_set_mode(uint16_t mode);
void neopixel_set_color(int color);
#ifdef __cplusplus
}
#endif
