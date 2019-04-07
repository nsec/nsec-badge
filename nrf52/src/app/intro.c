//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include "main_menu.h"
#include <string.h>
#include "nrf_delay.h"

#include "images/neurosoft_logo_bitmap.h"
#include "images/neurosoft_circle_bitmap.h"
#include "images/neurosoft_name_bitmap.h"
#include "images/beyond_reality_bitmap.h"

extern uint16_t gfx_width;
extern uint16_t gfx_height;


// The "line" represent slice of 3 pixel. it was manually extracted...
// Not the best but the logic to this more automaticaly was not even readable
struct line {
    uint8_t start_x;
    uint8_t start_y;
    uint8_t len;
};

static struct line brain[15] = {
    {25, 1, 24},
    {20, 4, 36},
    {15, 7, 45},
    {15, 10, 48},
    {18, 13, 45},
    {19, 16, 45},
    {21, 19, 45},
    {20, 22, 45},
    {19, 25, 45},
    {17, 28, 48},
    {13, 31, 51},
    {14, 34, 48},
    {26, 37, 36},
    {44, 40, 18},
    {47, 43, 6}
};

static void animated_brain(void)
{
    uint8_t bytes_per_line = neurosoft_logo_bitmap.width * 2;
    uint8_t offset_x = (gfx_width - neurosoft_logo_bitmap.width)/2;
    uint8_t offset_y = (gfx_height - neurosoft_logo_bitmap.height)/2;

    // iterate over the list lines
    for (int i = 0; i < 15; i++) {
        uint8_t start_x = brain[i].start_x;
        uint8_t start_y = brain[i].start_y;
        uint8_t len = brain[i].len;

        // Start by drawing a white square where we want to draw the real pixel
        for (int x = start_x; x < len + start_x; x+=3) {
            gfx_fill_rect(x + offset_x, start_y + offset_y, 3, 3, DISPLAY_WHITE);
            nrf_delay_ms(10);

            // Draw the 9 pixels
            for (int l = 0; l < 3; l++) {
                for (int k = 0; k < 3; k++) {
                    uint16_t index = ((bytes_per_line * (start_y + l) + (x + k) * 2));
                    uint8_t color_hi = neurosoft_logo_bitmap.image[index];
                    uint8_t color_lo = neurosoft_logo_bitmap.image[index + 1];
                    uint16_t color = (color_hi << 8) + color_lo;
                    display_draw_pixel(x + k + offset_x, start_y + l + offset_y,
                        color);
                }
            }
            nrf_delay_ms(10);
        }
    }
}

static void animated_name(const struct bitmap *bitmap)
{
    uint8_t bytes_per_line = bitmap->width * 2;
    uint8_t offset_x = (gfx_width - bitmap->width)/2;
    uint8_t offset_y = 60;

    for (int x = 0; x < gfx_width; x += 4) {
        gfx_fill_rect(x, offset_y, 4, 10, DISPLAY_WHITE);
        nrf_delay_ms(30);
        if (x < offset_x || x >= (offset_x + bitmap->width)) {
            gfx_fill_rect(x, offset_y, 4, 10, DISPLAY_BLACK);
        } else {
            for (int l = 0; l < 10; l++) {
                for (int k = 0; k < 4; k++) {
                    uint16_t index = (bytes_per_line * l + (k + x - offset_x) * 2);
                    uint8_t color_hi = bitmap->image[index];
                    uint8_t color_lo = bitmap->image[index + 1];
                    uint16_t color = (color_hi << 8) + color_lo;
                    display_draw_pixel(k + x, l + offset_y, color);
                }
            }
        }
    }
}

//TODO improve with some neopixeling ?
static void nsec_ctf_intro(void)
{
    gfx_draw_16bit_bitmap(47, 22, &neurosoft_circle_bitmap, DISPLAY_BLACK);
    nrf_delay_ms(500);

    animated_brain();
    nrf_delay_ms(100);

    animated_name(&neurosoft_name_bitmap);
    nrf_delay_ms(100);

    animated_name(&beyond_reality_bitmap);
    nrf_delay_ms(100);
}

static void nsec_conf_intro(void)
{
    //todo
}

void nsec_intro(void)
{
#ifdef NSEC_FLAVOR_CTF
    nsec_ctf_intro();
#else
    nsec_conf_intro();
#endif
}
