//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include <app_timer.h>

#include <string.h>

#include "drivers/ws2812fx.h"
#include "drivers/display.h"
#include "drivers/flash.h"
#include "drivers/controls.h"

#include "timer.h"
#include "app_soldering.h"
#include "gfx_effect.h"
#include "application.h"
#include "nsec_led_settings.h"

#include "images/flames_bitmap.h"

#define SOLDERING_FLAG_FLASH_ADDR 0x07F000

APP_TIMER_DEF(m_soldering_track);
char flash_flag[24];
uint8_t next_action;
uint8_t success = 0;

static void print_button_flag(void)
{
    gfx_set_cursor(1, 17);
    gfx_fill_rect(0, 16, 160, 28, DISPLAY_BLACK);
    gfx_puts("FLAG-2552ab1c-4eae-4728\n");
}

static void print_next_action(void)
{
    do {
        next_action = nsec_random_get_byte(7);
    } while (!is_press_action(next_action));

    gfx_set_cursor(1, 30);
    gfx_fill_rect(0, 16, 80, 28, DISPLAY_BLACK);
    gfx_puts("Press: ");

    switch(next_action) {
        case BUTTON_UP:
            gfx_puts("UP");
            break;
        case BUTTON_DOWN:
            gfx_puts("DOWN");
            break;
        case BUTTON_BACK:
            gfx_puts("BACK");
            break;
        case BUTTON_ENTER:
            gfx_puts("ENTER");
            break;
    }

    char val[12];
    sprintf(val, "Success: %d", success);
    gfx_set_cursor(80, 30);
    gfx_puts(val);
}

static void soldering_button_handler(button_t button)
{
    if (success < 10) {
        if (button == next_action) {
            success++;
            if (success == 10) {
                print_button_flag();
            } else {
                print_next_action();
            }
        } else {
            if (is_press_action(button)) {
                success = 0;
                print_next_action();
            }
        }
    }
}

static void show_soldering_flag(void * p_context)
{
    gfx_fill_rect(0, 0, 160, 36, DISPLAY_BLACK);
    gfx_set_text_size(1);
    gfx_set_cursor(1, 1);
    gfx_puts("FLAG-ec88d77d-2937-4c2c\n");
    gfx_puts(flash_flag);
    gfx_puts("\n");

    print_next_action();
}

/*static void test_flash(void)
{
    uint8_t data[128];
    ret_code_t ret = flash_erase(SOLDERING_FLAG_FLASH_ADDR);
    APP_ERROR_CHECK(ret);

    strcpy((char*)data, "FLAG-15a5cb08-39aa-4fca");

    ret = flash_write_128(SOLDERING_FLAG_FLASH_ADDR, data);
    APP_ERROR_CHECK(ret);
}*/

static void init_soldering_track(void) {
    uint8_t data[128];
    const uint32_t colors[3] = {ORANGE, RED, GREEN};

    //test_flash();
    start_WS2812FX();
    resetSegments_WS2812FX();

    setSegment_color_array_WS2812FX(0, 0, NEOPIXEL_COUNT - 1, FX_MODE_FIRE_FLICKER_INTENSE,
                         colors, MEDIUM_SPEED, false);

    gfx_fill_screen(DISPLAY_BLACK);

    gfx_set_cursor(26, 2);
    gfx_set_text_size(2);
    gfx_set_text_background_color(DISPLAY_RED, DISPLAY_BLACK);
    gfx_puts("Soldering");
    gfx_set_cursor(40, 22);
    gfx_puts("Track !");


    for (int i = 0; i < 6; i++) {
        gfx_draw_16bit_bitmap(i * 26, 44, &flames_bitmap, DISPLAY_BLACK );
    }

    ret_code_t err_code = app_timer_create(&m_soldering_track,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                show_soldering_flag);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_soldering_track, APP_TIMER_TICKS(5000), NULL);
    APP_ERROR_CHECK(err_code);

    err_code = flash_read_128(SOLDERING_FLAG_FLASH_ADDR, data);
    APP_ERROR_CHECK(err_code);

    if (strncmp("FLAG", (char*)data, 4) == 0) {
        strncpy(flash_flag, (char*)data, 24);
        flash_flag[23] = '\0';
    } else {
        strcpy(flash_flag, "No flash...");
    }

    nsec_controls_add_handler(soldering_button_handler);
}

void app_soldering(void (*service_callback)()) {
    init_soldering_track();

    while (application_get() == app_soldering) {
        service_callback();
    }

    nsec_controls_suspend_handler(soldering_button_handler);
}
