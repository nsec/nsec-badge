//  Copyright (c) 2018
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef neoPixel_h
#define neoPixel_h

#include <stdint.h>

#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2))
#define MAGIC_T0H 6UL | (0x8000)  // 0.375us
#define MAGIC_T1H 13UL | (0x8000) // 0.8125us
#define CTOPVAL 20UL              // 1.25us

#define CYCLES_800_T0H 18 // ~0.36 us
#define CYCLES_800_T1H 41 // ~0.76 us
#define CYCLES_800 71     // ~1.25 us

#define NEOPIXEL_COUNT 15

void nsec_neoPixel_init(void);
void nsec_neoPixel_clear(void);
void nsec_neoPixel_set_pixel_color(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void nsec_neoPixel_set_pixel_color_packed(uint16_t n, uint32_t c);
uint32_t nsec_neoPixel_get_pixel_color(uint16_t n);
void nsec_neoPixel_show(void);
uint8_t nsec_neoPixel_get_brightness(void);
void nsec_neoPixel_set_brightness(uint8_t b);

#endif /* neoPixel_h */
