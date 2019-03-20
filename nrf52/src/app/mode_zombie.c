//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)


#include <app_timer.h>

#include "drivers/ws2812fx.h"
#include "drivers/nsec_storage.h"
#include "drivers/display.h"

#include "timer.h"
#include "mode_zombie.h"
#include "persistency.h"
#include "home_menu.h"
#include "status_bar.h"

APP_TIMER_DEF(m_zombie_timer_id);
static bool process_mode_zombie = false;

/*
 * Callback function
 */
static void mode_zombie_timer_handler(void *p_context)
{
    process_mode_zombie = true;
}

static void mode_zombie_glitch(void)
{
    uint64_t start = get_current_time_millis();
    uint64_t elapse = 0;

    resetSegments_WS2812FX();
    setMode_WS2812FX(FX_MODE_BLINK);
    setArrayColor_packed_WS2812FX(RED, 0);
    setArrayColor_packed_WS2812FX(ORANGE, 1);
    setSpeed_WS2812FX(100);

    while (elapse <= 15000) { //15 sec
        uint8_t x = nsec_random_get_byte(160);
        uint8_t y = nsec_random_get_byte(80);
        uint16_t color = nsec_random_get_u16(0xFFFF);
        display_draw_pixel(x, y ,color);
        service_WS2812FX();
        elapse = get_current_time_millis() - start;
    }

}

void mode_zombie_process(void)
{
    static uint32_t call_count = 0;
    uint32_t odds_modifier = 0;

    if (process_mode_zombie) {
        process_mode_zombie = false;
        call_count++;
    } else {
        return;
    }

    /* Based on a 8 hours badge utilisation that code
    will be executed 17280 time during the conference.
    If we want the mode zombie to trigger 3 times during
    the conference we need 17280 / 3 = 5760
    */

    odds_modifier = get_persist_zombie_odds_modifier();

    /* Increase the odds after one hour of uptime
    If the badge stay open for 24 hours, the odds will become 100%
    */
    if (call_count >= 720) {
        call_count = 0;
        odds_modifier += 240;
        set_persist_zombie_odds_modifier(odds_modifier);
    }

    uint16_t rand = nsec_random_get_u16((5760 - odds_modifier) + 1);

    if (rand == 1) {
        mode_zombie_glitch();

        // Reset the odds
        set_persist_zombie_odds_modifier(0);

        // restore badge
        load_stored_led_settings();
        show_home_menu(HOME_STATE_MENU);
        nsec_status_bar_ui_redraw();
    }
}

/*
 * Initialize the app timer library and heartbeat
 */
void mode_zombie_init(void) {
    ret_code_t err_code;

    // Create the heartbeat timer
    err_code = app_timer_create(&m_zombie_timer_id,
                APP_TIMER_MODE_REPEATED,
                mode_zombie_timer_handler);
    APP_ERROR_CHECK(err_code);

    // Start the heartbeat timer
    err_code = app_timer_start(m_zombie_timer_id,
                APP_TIMER_TICKS(ZOMBIE_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);
}
