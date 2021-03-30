#ifndef __NEOPIXEL_C_H_
#define __NEOPIXEL_C_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void neopixel_set_brightness(uint8_t brightness);
void neopixel_set_mode(uint16_t mode);
void neopixel_set_color(int color);

#ifdef __cplusplus
}
#endif


#endif // __NEOPIXEL_C_H_
