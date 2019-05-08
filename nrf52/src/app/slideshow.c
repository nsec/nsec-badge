#include <string.h>

#include <nrf_delay.h>

#include "drivers/controls.h"
#include "drivers/display.h"

#include "app_screensaver.h"
#include "application.h"
#include "gfx_effect.h"
#include "gui.h"
#include "random.h"
#include "slideshow.h"

enum slideshow_text_type {
    SLIDESHOW_TEXT_DESC,
    SLIDESHOW_TEXT_DESC_FINAL,
    SLIDESHOW_TEXT_RESULT_PASS,
    SLIDESHOW_TEXT_RESULT_WARN,
    SLIDESHOW_TEXT_RESULT_FAIL,
    SLIDESHOW_TEXT_RESOLUTION,
};

typedef struct SlideshowTextJob {
    char *text;
    enum slideshow_text_type type;
    uint8_t line;
} Job;

static uint8_t slideshow_title_indicator = 0;

static void slideshow_render_title_indicator()
{
    if (slideshow_title_indicator == 1) {
        gfx_fill_circle(15, 9, 4, DISPLAY_RED);
    } else if (slideshow_title_indicator == 36) {
        gfx_fill_rect(10, 5, 10, 10, SLIDESHOW_TITLE_BG);
    } else if (slideshow_title_indicator > 72) {
        slideshow_title_indicator = 0;
    }

    slideshow_title_indicator++;
}

static void slideshow_button_handler(button_t button)
{
    application_clear();
}

void slideshow_app(void (*service_device)())
{
    uint16_t timeout = 0;

    nsec_controls_add_handler(slideshow_button_handler);

    gfx_fill_rect(0, 0, DISPLAY_HEIGHT, DISPLAY_WIDTH, 0);
    gfx_fill_rect(0, 0, DISPLAY_HEIGHT, 20, SLIDESHOW_TITLE_BG);

    gfx_set_text_background_color(DISPLAY_WHITE, SLIDESHOW_TITLE_BG);
    gfx_set_cursor(32, 6);
    gfx_puts("System health check");

    slideshow_render_title_indicator();

    uint8_t jobs_count;
    struct SlideshowTextJob jobs[10] = {0};

    switch (nsec_random_get_byte(6)) {
    case 0:
        jobs_count = 5;
        jobs[0] = (Job){"Checking available", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"memory", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){"   low  ", SLIDESHOW_TEXT_RESULT_FAIL, 0};
        jobs[3] =
            (Job){"You have low free space", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] = (Job){"Erasing memories...", SLIDESHOW_TEXT_RESOLUTION, 1};
        break;

    case 1:
        jobs_count = 5;
        jobs[0] = (Job){"Checking available", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"memory", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){" warning", SLIDESHOW_TEXT_RESULT_WARN, 0};
        jobs[3] = (Job){"Performing memory", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] = (Job){"defragmentation...", SLIDESHOW_TEXT_RESOLUTION, 1};
        break;

    case 2:
        jobs_count = 3;
        jobs[0] = (Job){"Checking available", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"memory", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){" nominal", SLIDESHOW_TEXT_RESULT_PASS, 0};
        break;

    case 3:
        jobs_count = 5;
        jobs[0] = (Job){"Measuring stress", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"level", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){" warning", SLIDESHOW_TEXT_RESULT_WARN, 0};
        jobs[3] = (Job){"High stress level.", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] = (Job){"Injecting endorphins.", SLIDESHOW_TEXT_RESOLUTION, 1};
        break;

    case 4:
        jobs_count = 5;
        jobs[0] = (Job){"Measuring stress", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"level", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){"elevated", SLIDESHOW_TEXT_RESULT_FAIL, 0};
        jobs[3] = (Job){"Consult a technician", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] =
            (Job){"at Ministry of Health", SLIDESHOW_TEXT_RESOLUTION, 1};
        break;

    case 5:
        jobs_count = 5;
        jobs[0] = (Job){"System uptime..........", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){" nominal", SLIDESHOW_TEXT_RESULT_PASS, 0};
        jobs[3] = (Job){"Do not forget to", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] = (Job){"get enough sleep.", SLIDESHOW_TEXT_RESOLUTION, 1};
        break;

    case 6:
        jobs_count = 9;
        jobs[0] = (Job){"Intercepting radio", SLIDESHOW_TEXT_DESC, 0};
        jobs[1] = (Job){"transmitions", SLIDESHOW_TEXT_DESC_FINAL, 1};
        jobs[2] = (Job){" blocked", SLIDESHOW_TEXT_RESULT_WARN, 0};
        jobs[3] =
            (Job){"Reminder: do not accept", SLIDESHOW_TEXT_RESOLUTION, 0};
        jobs[4] = (Job){"unauthorized illegal", SLIDESHOW_TEXT_RESOLUTION, 1};
        jobs[5] = (Job){"radio transmittions", SLIDESHOW_TEXT_RESOLUTION, 2};
        jobs[6] = (Job){"from unknown parties.", SLIDESHOW_TEXT_RESOLUTION, 3};
        jobs[7] = (Job){"They may have unknown", SLIDESHOW_TEXT_RESOLUTION, 4};
        jobs[8] = (Job){"effects on your body.", SLIDESHOW_TEXT_RESOLUTION, 5};
        break;

    default:
        return;
    }

    for (uint8_t i = 0; i < jobs_count; i++) {
        enum slideshow_text_type type = jobs[i].type;

        timeout = 50;
        while (timeout-- > 0) {
            service_device();
            slideshow_render_title_indicator();
        }

        switch (type) {
        case SLIDESHOW_TEXT_DESC:
        case SLIDESHOW_TEXT_DESC_FINAL:
            gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
            gfx_set_cursor(10, 25 + jobs[i].line * 13);
            gfx_puts(jobs[i].text);

            if (type == SLIDESHOW_TEXT_DESC_FINAL) {
                timeout = (22 - strlen(jobs[i].text) - 8) * 5 + 1;

                for (uint8_t j = 0; j < timeout; j++) {
                    if (j % 5 == 0) {
                        gfx_puts(".");
                    }

                    service_device();
                    slideshow_render_title_indicator();
                }
            }

            break;

        case SLIDESHOW_TEXT_RESULT_FAIL:
        case SLIDESHOW_TEXT_RESULT_PASS:
        case SLIDESHOW_TEXT_RESULT_WARN:
            if (type == SLIDESHOW_TEXT_RESULT_FAIL) {
                gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_RED);
            } else if (type == SLIDESHOW_TEXT_RESULT_PASS) {
                gfx_set_text_background_color(DISPLAY_WHITE,
                                              SLIDESHOW_TITLE_BG);
            } else {
                gfx_set_text_background_color(DISPLAY_BLACK, DISPLAY_YELLOW);
            }

            gfx_puts(jobs[i].text);
            break;

        case SLIDESHOW_TEXT_RESOLUTION:
            gfx_set_text_background_color(DISPLAY_WHITE, DISPLAY_BLACK);
            gfx_fill_rect(0, 55 + (jobs[i].line % 2) * 13, DISPLAY_HEIGHT, 13,
                          DISPLAY_BLACK);
            gfx_set_cursor(10, 55 + (jobs[i].line % 2) * 13);
            gfx_puts(jobs[i].text);
            break;
        }

        service_device();
    }

    timeout = 400;
    while (application_get() == slideshow_app) {
        if (timeout == 0) {
            application_set(app_screensaver_sleep);
        }

        service_device();
        slideshow_render_title_indicator();

        timeout--;
    };

    nsec_controls_clear_handlers();
    screensaver_reset();
}
