//  Copyright (c) 2018-2019
//  NorthSec badge team <https://github.com/nsec>
//
//  License: MIT (see LICENSE for details)

#include <string.h>

#include <nordic_common.h>

#include "nsec_games_menu.h"

#include "drivers/buttons.h"
#include "drivers/controls.h"
#include "drivers/display.h"

#include "application.h"
#include "game_mines.h"
#include "game_snake.h"
#include "gfx_effect.h"
#include "gui.h"
#include "home_menu.h"
#include "main_menu.h"
#include "menu.h"

#include "FreeRTOS.h"
#include "queue.h"

static menu_t menu;

static void nsec_games_start_cortexviper_application(uint8_t item)
{
    application_set(&snake_application);
}

static void nsec_games_start_mindsweeper_application(uint8_t item)
{
    application_set(&mines_application);
}

static menu_item_s nsec_games_menu_items[] = {
    {.label = "Cortex viper",
     .handler = nsec_games_start_cortexviper_application},

    {.label = "Mindsweeper",
     .handler = nsec_games_start_mindsweeper_application}};

static void draw_games_title(void)
{
    struct title title;
    title.pos_y = 5;
    title.pos_x = 37;
    title.text_color = DISPLAY_BLUE;
    title.bg_color = DISPLAY_WHITE;
    strcpy(title.text, "GAMES");
    draw_title(&title);
}

static bool nsec_games_menu_button_handler(button_t button)
{
    bool quit = false;

    if (button == BUTTON_BACK) {
        quit = true;
    } else {
        menu_button_handler(&menu, button);
    }

    return quit;
}

void nsec_games_menu_show(void)
{
    draw_games_title();

    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);

    menu_init(&menu, CONF_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(nsec_games_menu_items), nsec_games_menu_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);

    while (true) {
        button_t btn;
        BaseType_t ret = xQueueReceive(button_event_queue, &btn, portMAX_DELAY);
        APP_ERROR_CHECK_BOOL(ret == pdTRUE);

        bool quit = nsec_games_menu_button_handler(btn);

        if (quit) {
            break;
        }
    }
}
