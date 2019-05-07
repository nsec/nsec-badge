//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "nsec_conf_schedule.h"
#include "cli.h"
#include "conf_sched.h"
#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "main_menu.h"
#include "menu.h"
#include "text_box.h"
#include "gui.h"
#include "gosecure_animation.h"
#include "utils.h"
#include <nordic_common.h>
#include <stdlib.h>
#include <string.h>

#ifdef BOARD_BRAIN
#define ROW_COUNT                   9 // 10 - status bar
#define COLUMN_COUNT                26
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT
#else
#define ROW_COUNT                   7 // 8 - status bar
#define COLUMN_COUNT                21
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT
#endif

#define PRESENTER_COUNT             40

extern uint16_t gfx_width;
extern uint16_t gfx_height;

const char *abstract_party =
    "Conference party  supported by Hackerone\n"
    "Where: Arcade Montreal 2031 Saint-Denis\n"
    "When: Thursday 16 May, 20h30-3h00";

const char *abstract_hack_mtl =
    "HACK MTL premiere screening\n"
    "Where: Cinema IMAX\n"
    "When: Thursday 16 May, 18h15-20h30\n"
    "We are in 2019 and privacy no longer exists. In less than one generation, "
    "the internet has become a mass surveillance machine. The business model "
    "that drives this machine can be summed up by a simple saying: If it’s "
    "free, you’re the product. Google, Facebook, Twitter, Skype and all these "
    "other services have become masters in the art of monetizing our "
    "interactions with their platforms. In this brave new world, we have "
    "become the primary resource of the surveillance economy; we have all "
    "become generators of personal data. All the traces we leave of ourselves "
    "on the internet are thus captured, stored and accessible to both States "
    "and web giants ... and this trend is rapidly intensifying. Whatever hope "
    "is left will come from the hacker movement. After all, the internet is "
    "their playground. Hackers know better than anyone the real extent of "
    "surveillance. According to these circuit subversives, Big Brother exists "
    "and only a technological struggle will be able to defeat him. Thus, they "
    "dismantle computers in search of backdoors. They climb rooftops to build "
    "a new alternative and community network. They develop encryption tools. "
    "They come together at major international meetings and actively promote "
    "online anonymity. By giving a voice to our local hacker scene, HAK_MTL "
    "explores Montreal’s contribution to the global fight for privacy.";

static struct text_box_config config = {
    CONF_POS_X,
    CONF_POS_Y,
    CONF_WIDTH,
    CONF_HEIGHT,
    HOME_MENU_BG_COLOR,
    DISPLAY_WHITE
};

static void nsec_schedule_button_handler(button_t button);
static void nsec_schedule_show_talks(uint8_t item);
static void nsec_schedule_show_speakers(uint8_t item);
static void nsec_schedule_show_events(uint8_t item);
static void nsec_schedule_show_gosecure_event(uint8_t item);
static void nsec_schedule_show_conference_party(uint8_t item);
static void nsec_schedule_show_hack_mtl(uint8_t item);

static menu_item_s days_schedule_items[] = {
    {
        .label = "Thursday, May 16th",
        .handler = nsec_schedule_show_talks
    },
    {
        .label = "Friday, May 17th",
        .handler = nsec_schedule_show_talks
    },
    {
        .label = "Speakers",
        .handler = nsec_schedule_show_speakers,
    }, {
        .label = "Events",
        .handler = nsec_schedule_show_events
    }
};


static menu_item_s events_schedule_items[] = {
    {
        .label = "GoSecure whisky tasting",
        .handler = nsec_schedule_show_gosecure_event
    },
    {
        .label = "Conference party",
        .handler = nsec_schedule_show_conference_party
    },
    {
        .label = "HACK MTL",
        .handler = nsec_schedule_show_hack_mtl
    }
};

enum schedule_state {
    SCHEDULE_STATE_CLOSED,
    SCHEDULE_STATE_DATES,
    SCHEDULE_STATE_TALKS,
    SCHEDULE_STATE_TALK_DETAILS,
    SCHEDULE_STATE_SPEAKERS,
    SCHEDULE_STATE_SPEAKER_DETAILS,
    SCHEDULE_STATE_EVENTS,
    SCHEDULE_STATE_EVENT_DESC,
};

static enum schedule_state schedule_state = SCHEDULE_STATE_CLOSED;

static void draw_conf_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 5;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "CONFERENCE");
    draw_title(&title);
}

void nsec_schedule_show_dates(void) {
    draw_conf_title();

    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);

    menu_init(CONF_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
        ARRAY_SIZE(days_schedule_items), days_schedule_items,
        HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    schedule_state = SCHEDULE_STATE_DATES;
    nsec_controls_add_handler(nsec_schedule_button_handler);
}

static void nsec_schedule_return_to_talks(void)
{
    schedule_state = SCHEDULE_STATE_TALKS;
    menu_open();
    menu_ui_redraw_all();
}

static void nsec_schedule_return_to_speakers(void)
{
    schedule_state = SCHEDULE_STATE_SPEAKERS;
    menu_open();
    menu_ui_redraw_all();
}

static const struct talk *cur_talks = NULL;
static int cur_num_talks = 0;

static char big_buf[10000];

static int format_talk_detailed(char *buf, const struct talk *t)
{
    return sprintf(buf, "%s\n%d:%02d to %d:%02d\n%s\n%s\n\n%s", t->title,
                   t->start_h, t->start_m, t->end_h, t->end_m, tracks[t->track],
                   t->names, t->abstract);
}

static void nsec_schedule_show_talk_details(uint8_t item)
{
    APP_ERROR_CHECK_BOOL(item < cur_num_talks);
    menu_close();

    const struct talk *t = &cur_talks[item];

    format_talk_detailed(big_buf, t);

    text_box_init(big_buf, &config);

    schedule_state = SCHEDULE_STATE_TALK_DETAILS;
}

static menu_item_s generated_menu[50];

static void prep_menu_talks(const struct talk *talks, int num_talks)
{
    char *p = big_buf;

    APP_ERROR_CHECK_BOOL(num_talks <= ARRAY_SIZE(generated_menu));
    for (int i = 0; i < num_talks; i++) {
        const struct talk *t = &talks[i];
        int n = sprintf(p, "%d:%02d %s", t->start_h, t->start_m, t->title);
        generated_menu[i].label = p;
        generated_menu[i].handler = nsec_schedule_show_talk_details;
        p += n + 1;
        APP_ERROR_CHECK_BOOL((p - big_buf) < sizeof(big_buf));
    }

    cli_printf("yo %d\r\n", p - big_buf);
}

static void nsec_schedule_show_talks(uint8_t item)
{
    cur_talks = item == 0 ? talks_16 : talks_17;
    cur_num_talks = item == 0 ? num_talks_16 : num_talks_17;

    prep_menu_talks(cur_talks, cur_num_talks);

    menu_init(CONF_POS, CONF_WIDTH, CONF_HEIGHT, cur_num_talks, generated_menu,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    schedule_state = SCHEDULE_STATE_TALKS;
}

static void nsec_schedule_show_speaker_details(uint8_t item)
{
    APP_ERROR_CHECK_BOOL(item < num_speakers);
    menu_close();

    text_box_init(speakers[item].bio, &config);

    schedule_state = SCHEDULE_STATE_SPEAKER_DETAILS;
}

static void prep_menu_speakers()
{
    APP_ERROR_CHECK_BOOL(num_speakers <= ARRAY_SIZE(generated_menu));
    for (int i = 0; i < num_speakers; i++) {
        generated_menu[i].label = speakers[i].name;
        generated_menu[i].handler = nsec_schedule_show_speaker_details;
    }
}

void nsec_schedule_show_speakers(uint8_t item)
{
    prep_menu_speakers();

    menu_init(CONF_POS, CONF_WIDTH, CONF_HEIGHT, num_speakers, generated_menu,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    schedule_state = SCHEDULE_STATE_TALKS;
}

void nsec_schedule_show_events(uint8_t item) {
    menu_init(CONF_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(events_schedule_items), events_schedule_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    schedule_state = SCHEDULE_STATE_EVENTS;
}

void nsec_schedule_show_gosecure_event(uint8_t item) {
    menu_close();
    schedule_state = SCHEDULE_STATE_CLOSED;

    application_set(gosecure_animation_app);
}

void nsec_schedule_show_conference_party(uint8_t item)
{
    menu_close();
    schedule_state = SCHEDULE_STATE_EVENT_DESC;
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    text_box_init(abstract_party, &config);
}

void nsec_schedule_show_hack_mtl(uint8_t item)
{
    menu_close();
    schedule_state = SCHEDULE_STATE_EVENT_DESC;
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    text_box_init(abstract_hack_mtl, &config);
}

static void nsec_schedule_button_handler(button_t button) {
    if(button == BUTTON_BACK) {
        switch (schedule_state) {
        case SCHEDULE_STATE_SPEAKER_DETAILS:
            nsec_schedule_return_to_speakers();
            break;
        case SCHEDULE_STATE_TALK_DETAILS:
            prep_menu_talks(cur_talks, cur_num_talks);
            nsec_schedule_return_to_talks();
            break;
        case SCHEDULE_STATE_TALKS:
        case SCHEDULE_STATE_SPEAKERS:
        case SCHEDULE_STATE_EVENTS:
            redraw_home_menu_burger_selected();
            nsec_schedule_show_dates();
            break;
        case SCHEDULE_STATE_EVENT_DESC:
            redraw_home_menu_burger_selected();
            nsec_schedule_show_events(3);
            break;
        case SCHEDULE_STATE_DATES:
            menu_close();
            schedule_state = SCHEDULE_STATE_CLOSED;
            show_main_menu();
            break;

        default:
            break;
        }
    }
}

// Make sure that all the talk details we can eventually generate fits into the
// static buffer.
void nsec_conf_length_check()
{
    for (int i = 0; i < num_talks_16; i++) {
        int n = format_talk_detailed(big_buf, &talks_16[i]);
        APP_ERROR_CHECK_BOOL(n < sizeof(big_buf));
    }
}
