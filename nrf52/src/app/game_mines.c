#include <stdint.h>

#include "application.h"
#include "drivers/display.h"

#include "images/external/mines_splash_bitmap.h"

#define MINES_GAME_GOTO(new_state) p_state->current_state = new_state

#define MINES_GAME_STATE_BOOT 1
#define MINES_GAME_STATE_BOOT_SPLASH 2
#define MINES_GAME_STATE_MENU 3
#define MINES_GAME_STATE_CONTROLS 4
#define MINES_GAME_STATE_MANUAL 5
#define MINES_GAME_STATE_SELECT_LEVEL 6
#define MINES_GAME_STATE_INIT_GAME 7
#define MINES_GAME_STATE_DRAW_CANVAS 8
#define MINES_GAME_STATE_GAME 9
#define MINES_GAME_STATE_EXPLOSION 10
#define MINES_GAME_STATE_EXPLOSION_MSG 11
#define MINES_GAME_STATE_CLEARED 12
#define MINES_GAME_STATE_CLEARED_MSG 13
#define MINES_GAME_STATE_POST_MENU 14
#define MINES_GAME_STATE_FLAG 15
#define MINES_GAME_STATE_EXIT 255

typedef struct MinesGameState {
    uint8_t current_state;
} MinesGameState;

static void mines_game_state_boot_handle(MinesGameState *p_state)
{
    display_draw_16bit_ext_bitmap(0, 0, &mines_splash_bitmap, 0);

    MINES_GAME_GOTO(MINES_GAME_STATE_BOOT_SPLASH);
}

static void mines_game_state_boot_splash_handle(MinesGameState *p_state) {}
static void mines_game_state_cleared_handle(MinesGameState *p_state) {}
static void mines_game_state_cleared_msg_handle(MinesGameState *p_state) {}
static void mines_game_state_controls_handle(MinesGameState *p_state) {}
static void mines_game_state_draw_canvas_handle(MinesGameState *p_state) {}
static void mines_game_state_explosion_handle(MinesGameState *p_state) {}
static void mines_game_state_explosion_msg_handle(MinesGameState *p_state) {}
static void mines_game_state_flag_handle(MinesGameState *p_state) {}
static void mines_game_state_game_handle(MinesGameState *p_state) {}
static void mines_game_state_init_game_handle(MinesGameState *p_state) {}
static void mines_game_state_manual_handle(MinesGameState *p_state) {}
static void mines_game_state_menu_handle(MinesGameState *p_state) {}
static void mines_game_state_post_menu_handle(MinesGameState *p_state) {}
static void mines_game_state_select_level_handle(MinesGameState *p_state) {}

void mines_application(void (*service_device)())
{
    MinesGameState state = {.current_state = MINES_GAME_STATE_BOOT};

    while (application_get() == mines_application) {
        service_device();

        switch (state.current_state) {
        case MINES_GAME_STATE_BOOT:
            mines_game_state_boot_handle(&state);
            break;

        case MINES_GAME_STATE_BOOT_SPLASH:
            mines_game_state_boot_splash_handle(&state);
            break;

        case MINES_GAME_STATE_CLEARED:
            mines_game_state_cleared_handle(&state);
            break;

        case MINES_GAME_STATE_CLEARED_MSG:
            mines_game_state_cleared_msg_handle(&state);
            break;

        case MINES_GAME_STATE_CONTROLS:
            mines_game_state_controls_handle(&state);
            break;

        case MINES_GAME_STATE_DRAW_CANVAS:
            mines_game_state_draw_canvas_handle(&state);
            break;

        case MINES_GAME_STATE_EXPLOSION:
            mines_game_state_explosion_handle(&state);
            break;

        case MINES_GAME_STATE_EXIT:
            application_clear();
            break;

        case MINES_GAME_STATE_EXPLOSION_MSG:
            mines_game_state_explosion_msg_handle(&state);
            break;

        case MINES_GAME_STATE_FLAG:
            mines_game_state_flag_handle(&state);
            break;

        case MINES_GAME_STATE_GAME:
            mines_game_state_game_handle(&state);
            break;

        case MINES_GAME_STATE_INIT_GAME:
            mines_game_state_init_game_handle(&state);
            break;

        case MINES_GAME_STATE_MANUAL:
            mines_game_state_manual_handle(&state);
            break;

        case MINES_GAME_STATE_MENU:
            mines_game_state_menu_handle(&state);
            break;

        case MINES_GAME_STATE_POST_MENU:
            mines_game_state_post_menu_handle(&state);
            break;

        case MINES_GAME_STATE_SELECT_LEVEL:
            mines_game_state_select_level_handle(&state);
            break;

        default:
            state.current_state = MINES_GAME_STATE_EXIT;
        }
    }

    application_clear();
}
