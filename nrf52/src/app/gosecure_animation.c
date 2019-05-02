#include <bitmap.h>

#include "drivers/display.h"

#include "application.h"
#include "gfx_effect.h"
#include "gosecure_animation.h"

#ifdef NSEC_FLAVOR_CONF

#include "gosecure_animation_sequences.h"
#include "images/external/conf/gosecure_1_back_bitmap.h"
#include "images/external/conf/gosecure_4_bitmap.h"

void gosecure_animation_app(void (*service_device)())
{
    uint16_t label_multiplier = 2;
    uint16_t label_offset = 75;

    uint16_t inter1_offset = 200;
    uint16_t inter2_offset = 350;
    uint16_t inter3_offset = 500;

    uint16_t pan_multiplier = 3;
    uint16_t pan_offset = 650;
    uint16_t pan_stop = 20;

    uint16_t counter = 0;

    while (application_get() == gosecure_animation_app) {
        if (counter == 0) {
            display_draw_16bit_ext_bitmap(0, 0, &gosecure_1_back_bitmap, 0);
        }

        if (counter >= label_offset &&
            counter < label_offset +
                          label_multiplier * gosecure_sequence_label_length) {

            uint16_t offset = counter - label_offset;

            if (offset % label_multiplier == 0) {
                const struct gosecure_sequence_frame frame =
                    gosecure_sequence_label[offset / label_multiplier];

                display_draw_16bit_ext_bitmap(frame.x, frame.y, frame.bitmap,
                                              0);
            }
        }

        if (counter >= inter1_offset &&
            counter < inter1_offset + gosecure_sequence_inter_1_length) {

            const struct gosecure_sequence_frame frame =
                gosecure_sequence_inter_1[counter - inter1_offset];

            display_draw_16bit_ext_bitmap(frame.x, frame.y, frame.bitmap, 0);
        }

        if (counter >= inter2_offset &&
            counter < inter2_offset + gosecure_sequence_inter_2_length) {

            const struct gosecure_sequence_frame frame =
                gosecure_sequence_inter_2[counter - inter2_offset];

            display_draw_16bit_ext_bitmap(frame.x, frame.y, frame.bitmap, 0);
        }

        if (counter >= inter3_offset &&
            counter < inter3_offset + gosecure_sequence_inter_3_length) {

            const struct gosecure_sequence_frame frame =
                gosecure_sequence_inter_3[counter - inter3_offset];

            display_draw_16bit_ext_bitmap(frame.x, frame.y, frame.bitmap, 0);
        }

        if (counter >= pan_offset &&
            counter < pan_offset +
                          pan_multiplier * gosecure_sequence_pan_length -
                          pan_stop) {

            uint16_t offset = counter - pan_offset;

            if (offset % pan_multiplier == 0) {
                for (uint8_t i = offset / pan_multiplier, j = 0;
                     i < gosecure_sequence_pan_length; i++, j++) {
                    const struct gosecure_sequence_frame frame_content =
                        gosecure_sequence_pan[i];
                    const struct gosecure_sequence_frame frame_position =
                        gosecure_sequence_pan[j];

                    display_draw_16bit_ext_bitmap(frame_position.x,
                                                  frame_position.y,
                                                  frame_content.bitmap, 0);
                }
            }
        }

        if (counter == 700) {
            gfx_set_text_background_color(DISPLAY_BLACK, DISPLAY_WHITE);

            gfx_set_cursor(82, 28);
            gfx_puts("Whisky");

            gfx_set_cursor(70, 43);
            gfx_puts("degustation");

            gfx_update();
        }

        if (counter == 800) {
            gfx_fill_rect(48, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH, DISPLAY_WHITE);

            gfx_set_cursor(49, 20);
            gfx_puts("Salon du president");

            gfx_set_cursor(55, 40);
            gfx_puts("Thursday, 16 May");

            gfx_set_cursor(64, 55);
            gfx_puts("1:30PM - 6PM");
        }

        if (counter == 1000) {
            gfx_fill_rect(48, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH, DISPLAY_WHITE);
            display_draw_16bit_ext_bitmap(53, 33, &gosecure_4_bitmap, 0);
        }

        if (counter == 1250) {
            application_clear();
        }

        counter++;
        service_device();
    }
}

#else

void gosecure_animation_app(void (*service_device)())
{
    application_clear();
}

#endif
