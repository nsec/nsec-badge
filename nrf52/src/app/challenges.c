//  Copyright (c) 2018-2019
//  Thomas Dupuy <thom4s.d@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <string.h>

#include "drivers/controls.h"
#include "drivers/display.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "input_box.h"
#include "main_menu.h"
#include "menu.h"

enum setting_state {
    SETTING_STATE_CLOSED,
    SETTING_STATE_MENU,
    SETTING_STATE_CHALLENGES,
};

static void challenges_handle_buttons(button_t button);
static void show_challenge_re101(uint8_t item);
static enum setting_state state = SETTING_STATE_CLOSED;

static menu_item_s chall_menu_items[] = {{
    .label = "Challenge re101",
    .handler = show_challenge_re101,
}};

static void draw_challenges_title(void) {
    struct title title;
    title.pos_y = 5;
    title.pos_x = 5;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "CHALLENGES");
    draw_title(&title);
}

void nsec_challenges_show(void) {
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    draw_challenges_title();
    menu_init(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(chall_menu_items), chall_menu_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);
    nsec_controls_add_handler(challenges_handle_buttons);
    state = SETTING_STATE_MENU;
}

static void show_challenge_re101(uint8_t item) {
    input_box_init("ENTER CODE", "F000");
    state = SETTING_STATE_CHALLENGES;
}

static void challenges_handle_buttons(button_t button) {
    if (button == BUTTON_BACK) {
        switch (state) {
        case SETTING_STATE_MENU:
            state = SETTING_STATE_CLOSED;
            menu_close();
            show_main_menu();
            break;
        case SETTING_STATE_CHALLENGES:
            menu_close();
            nsec_challenges_show();
            break;
        default:
            break;
        }
    }
}
