//
//  gfx_effect.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-02-26.
//
//

#include "gfx_effect.h"
#include "ssd1306.h"
#include "random.h"

void nsec_gfx_effect_addNoise(uint8_t noise_amount) {
    for(int16_t y = 0; y < SSD1306_LCDHEIGHT; y++) {
        for(int16_t x = 0; x < SSD1306_LCDWIDTH; x++) {
            uint8_t r;
            nsec_random_get(&r, 1);
            if(r < noise_amount) {
                ssd1306_drawPixel(x, y, INVERSE);
            }
        }
    }
}
