#include "drivers/display.h"

#include "application.h"

#if defined(NSEC_FLAVOR_CTF) && !defined(SOLDERING_TRACK)

#include "images/external/ctf/resistance_slideshow_1_base_bitmap.h"
#include "images/external/ctf/resistance_slideshow_1_overlay_bitmap.h"
#include "images/external/ctf/resistance_slideshow_2_bitmap.h"
#include "images/external/ctf/resistance_slideshow_3_bitmap.h"
#include "images/external/ctf/resistance_slideshow_4_bitmap.h"

void resistance_slideshow_app(void (*service_device)())
{
    uint16_t step = 0;

    while (application_get() == resistance_slideshow_app) {
        if (step == 0) {
            display_draw_16bit_ext_bitmap(
                0, 0, &resistance_slideshow_1_base_bitmap, 0);
        }

        if (step < 720) {
            display_draw_16bit_ext_bitmap(
                360 - step, 41, &resistance_slideshow_1_overlay_bitmap, 0);
        }

        if (step == 720) {
            display_draw_16bit_ext_bitmap(
                0, 0, &resistance_slideshow_2_bitmap, 0);
        }

        if (step == 1100) {
            display_draw_16bit_ext_bitmap(
                0, 0, &resistance_slideshow_3_bitmap, 0);
        }

        if (step == 1500) {
            display_draw_16bit_ext_bitmap(
                0, 0, &resistance_slideshow_4_bitmap, 0);
        }


        service_device();
        step++;

        if (step >= 2000) {
            break;
        }
    }

    application_clear();
}

#else
void resistance_slideshow_app(void (*service_device)())
{
    application_clear();
}
#endif
