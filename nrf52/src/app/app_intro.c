//  Copyright (c) 2017
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "drivers/controls.h"
#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "cli.h"
#include "gfx_effect.h"
#include "gui.h"
#include "main_menu.h"
#include <string.h>
#include "nrf_delay.h"
#include "app_intro.h"
#include "persistency.h"
#include "timer.h"

#include "images/neurosoft_logo_bitmap.h"
#include "images/neurosoft_circle_bitmap.h"
#include "images/neurosoft_name_bitmap.h"
#include "images/beyond_reality_bitmap.h"
#include "images/avatar_neuro_bitmap.h"

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

static void service_process(void)
{
    cli_process();
    service_WS2812FX();
}

static void service_delay_ms(uint32_t ms)
{
    if (ms <= 50) {
        service_process();
        nrf_delay_ms(ms);
        service_process();
    } else {
        int64_t remaining = ms;
        do {
            uint64_t start = get_current_time_millis();
            service_process();
            remaining -= get_current_time_millis() - start;
        } while (remaining > 0);
    }
}

static void animated_brain(void)
{
    uint16_t gfx_width = gfx_get_screen_width();
    uint16_t gfx_height = gfx_get_screen_height();
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
            service_delay_ms(10);

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
            service_delay_ms(10);
        }
    }
}

static void animated_name(const struct bitmap *bitmap)
{
    uint16_t gfx_width = gfx_get_screen_width();
    uint8_t bytes_per_line = bitmap->width * 2;
    uint8_t offset_x = (gfx_width - bitmap->width)/2;
    uint8_t offset_y = 60;

    for (int x = 0; x < gfx_width; x += 4) {
        gfx_fill_rect(x, offset_y, 4, 10, DISPLAY_WHITE);
        service_delay_ms(30);
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

static void greetings(void)
{
    gfx_draw_16bit_bitmap(12, 25, &avatar_neuro_bitmap, DISPLAY_BLACK);
    service_delay_ms(500);
    gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
    gfx_set_cursor(63, 30);
    gfx_puts_lag("Greetings,", 20);
    service_delay_ms(500);
    gfx_set_cursor(63, 42);
    gfx_puts_lag(get_stored_identity(), 20);
}

#ifdef NSEC_FLAVOR_CTF
//TODO improve with some neopixeling ?
void app_intro(void (*service_callback)())
{
    gfx_draw_16bit_bitmap(47, 22, &neurosoft_circle_bitmap, DISPLAY_BLACK);
    service_delay_ms(500);

    animated_brain();
    service_delay_ms(100);

    animated_name(&neurosoft_name_bitmap);
    service_delay_ms(100);

    animated_name(&beyond_reality_bitmap);
    service_delay_ms(100);

    display_fill_screen_black();
    greetings();
    service_delay_ms(3000);

    /* Return to default app */
    application_clear();
}
#else
void app_intro(void (*service_callback)())
{
    greetings();
    service_delay_ms(1000);

    /* Return to default app */
    application_clear();
}
#endif
