#include "drivers/controls.h"
#include "drivers/display.h"

#include "app_screensaver.h"
#include "application.h"
#include "gfx_effect.h"
#include "gui.h"
#include "nsec_conf_slideshow.h"

#include "images/globe_bitmap.h"
#include "images/talos_logo_bitmap.h"

static inline void clear_slideshow_area()
{
    gfx_fill_rect(0, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH, DISPLAY_BLACK);
}

static void nsec_conf_slideshow_button_handler(button_t button)
{
    application_clear();
}

void nsec_conf_slideshow_app(void (*service_device)())
{
    nsec_controls_add_handler(nsec_conf_slideshow_button_handler);

    uint16_t i = 0;
    uint8_t repeat = 3;

    while (application_get() == nsec_conf_slideshow_app) {
        if (i == 0) {
            clear_slideshow_area();

            gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
            gfx_set_cursor(38, 17);
            gfx_puts("NorthSec 2019");

            gfx_fill_rect(46, 37, 60, 13, DISPLAY_WHITE);
            gfx_set_text_background_color(DISPLAY_BLACK, DISPLAY_WHITE);
            gfx_set_cursor(49, 40);
            gfx_puts("May 12-19");

            gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
            gfx_set_cursor(53, 55);
            gfx_puts("Montreal");

            gfx_update();
        }

        if (i == 200) {
            clear_slideshow_area();

            gfx_set_cursor(18, 21);
            gfx_puts_lag("Use built-in CLI", 30);
            service_device();

            gfx_set_cursor(18, 36);
            gfx_puts_lag("to view the", 30);
            service_device();

            gfx_set_cursor(18, 51);
            gfx_puts_lag("conference schedule.", 30);
            service_device();
        }

        if (i == 400) {
            clear_slideshow_area();
            gfx_draw_16bit_bitmap(18, 25, &talos_logo_bitmap, 0);
        }

        if (i == 550) {
            clear_slideshow_area();
            gfx_draw_16bit_bitmap(16, 25, &globe_bitmap, 0);

            gfx_set_cursor(40, 28);
            gfx_puts("https://nsec.io/");

            gfx_set_cursor(40, 48);
            gfx_puts("#nsec19");

            gfx_update();
        }

        if (i > 650) {
            if (repeat > 0) {
                i = 0;
                repeat--;
            } else {
                application_set(app_screensaver_sleep);
            }
        }

        i++;
        service_device();
    }

    nsec_controls_clear_handlers();
    screensaver_reset();
}
