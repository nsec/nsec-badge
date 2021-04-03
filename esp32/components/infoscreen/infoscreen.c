#include "freertos/FreeRTOS.h"

#include "buttons.h"
#include "graphics.h"

#include "infoscreen.h"

#define INFOSCREEN_POLE_WIDTH 8
#define INFOSCREEN_SCROLLBAR_BAR 234
#define INFOSCREEN_SCROLLBAR_HEIGHT 86

static const int infoscreen_style_warn[] = {
    LIBRARY_IMAGE_INFSC_POLE_WARN_0,    LIBRARY_IMAGE_INFSC_POLE_WARN_1,
    LIBRARY_IMAGE_INFSC_POLE_WARN_2,    LIBRARY_IMAGE_INFSC_POLE_WARN_3,
    LIBRARY_IMAGE_INFSC_POLE_WARN_4,    LIBRARY_IMAGE_INFSC_POLE_WARN_5,
    LIBRARY_IMAGE_INFSC_SCROLLBAR_WARN,
};

static void infoscreen_render(const char *message, const int message_height,
                              const int *style)
{
    int message_y = 0;
    bool need_update = true;
    button_t button;
    int pole_index = 0;

    while (true) {
        if (need_update) {
            if (message_y > 0)
                message_y = 0;

            if (-message_y + DISPLAY_HEIGHT > message_height)
                message_y = -(message_height - DISPLAY_HEIGHT);

            graphics_draw_jpeg(message, INFOSCREEN_POLE_WIDTH, message_y);

            int bar_y = (-message_y) *
                        (DISPLAY_HEIGHT - INFOSCREEN_SCROLLBAR_HEIGHT) /
                        (message_height - DISPLAY_HEIGHT);

            graphics_draw_from_library(style[6], INFOSCREEN_SCROLLBAR_BAR,
                                       bar_y);
        }

        for (int pole_y = 0; pole_y < DISPLAY_HEIGHT; pole_y += 24)
            graphics_draw_from_library(style[pole_index], 0, pole_y);

        pole_index = (pole_index + 1) % 6;

        button = BUTTON_NONE;
        xQueueReceive(button_event_queue, &button, 60 / portTICK_PERIOD_MS);

        switch (button) {
        case BUTTON_BACK_RELEASE:
        case BUTTON_ENTER_RELEASE:
            return;

        case BUTTON_DOWN:
            message_y -= DISPLAY_HEIGHT * 0.6;
            need_update = true;
            break;

        case BUTTON_UP:
            message_y += DISPLAY_HEIGHT * 0.6;
            need_update = true;
            break;

        default:
            need_update = false;
        }

        graphics_update_display();
    }
}

void infoscreen_display_bootwarning()
{
    infoscreen_render("/spiffs/infoscreen/bootwarning.jpeg", 480,
                      infoscreen_style_warn);

    for (int i = 0; i < 66; i += 7) {
        graphics_fadeout_display_buffer(i);
        graphics_update_display();
    }
}
