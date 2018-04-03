//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "gfx_effect.h"
#include "ssd1306.h"
#include "random.h"

void nsec_gfx_effect_addNoise(uint8_t noise_amount) {
    for(int16_t y = 0; y < SSD1306_LCDHEIGHT; y++) {
        for(int16_t x = 0; x < SSD1306_LCDWIDTH; x++) {
            uint8_t r;
            nsec_random_get(&r, 1);
            if(r < noise_amount) {
                ssd1306_drawPixel(x, y, SSD1306_INVERSE);
            }
        }
    }
}
