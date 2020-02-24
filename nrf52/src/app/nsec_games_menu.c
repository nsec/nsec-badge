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

static menu_t g_menu;

static void nsec_games_start_cortexviper_application(uint8_t item)
{
    // application_set(&snake_application);
}

static void nsec_games_start_mindsweeper_application(uint8_t item)
{
    // application_set(&mines_application);
}

static menu_item_s nsec_games_menu_items[] = {
    {.label = "Cortex viper",
     .handler = nsec_games_start_cortexviper_application},

    {.label = "Mindsweeper",
     .handler = nsec_games_start_mindsweeper_application}};

static void draw_games_title(void)
{
    draw_title("GAMES", 37, 5, DISPLAY_BLUE, DISPLAY_WHITE);
}

static bool games_menu_page_handle_button(button_t button)
{
    if (button == BUTTON_BACK) {
        return true;
    }

    menu_button_handler(&g_menu, button);

    return false;
}

static void games_menu_page_redraw(void)
{
    draw_games_title();
    gfx_fill_rect(GEN_MENU_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT, DISPLAY_WHITE);
    menu_ui_redraw_all(&g_menu);
}

static void games_menu_page_init(void *data)
{
    menu_init(&g_menu, CONF_POS, GEN_MENU_WIDTH, GEN_MENU_HEIGHT,
              ARRAY_SIZE(nsec_games_menu_items), nsec_games_menu_items,
              HOME_MENU_BG_COLOR, DISPLAY_WHITE);
}

const ui_page games_menu_page = {
    .init = games_menu_page_init,
    .redraw = games_menu_page_redraw,
    .handle_button = games_menu_page_handle_button,
};
