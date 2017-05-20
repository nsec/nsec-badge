//
//  nsec_nearby_badges.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2017-05-15.
//
//

#include "nsec_nearby_badges.h"
#include "ble/nsec_ble.h"
#include "led_effects.h"

#include <string.h>
#include <app_timer.h>

#include "ssd1306.h"
#include "app_glue.h"
#include "identity.h"

static struct {
    uint16_t id;
    uint8_t timeout_in_sec;
} _nearby_badges[NSEC_MAX_NEARBY_BADGES_COUNT];

APP_TIMER_DEF(nearby_timer);

static void nsec_nearby_each_second(void * context);
static void nsec_nearby_update_led_pettern(void);

void nsec_nearby_badges_init(void) {
    memset(_nearby_badges, 0, sizeof(_nearby_badges));
    app_timer_create(&nearby_timer, APP_TIMER_MODE_REPEATED, nsec_nearby_each_second);
    app_timer_start(nearby_timer, APP_TIMER_TICKS(1000, 0), NULL);
    nsec_ble_set_scan_callback(nsec_nearby_badges_callback);
}

static void nsec_nearby_each_second(void * context) {
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec > 0) {
            _nearby_badges[i].timeout_in_sec--;
        }
    }
    nsec_nearby_update_led_pettern();
    if(is_at_main_menu) {
        nsec_identity_update_nearby();
        gfx_update();
    }
}

void nsec_nearby_badges_callback(uint16_t badge_id, uint8_t addr[], int8_t rssi) {
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].id == badge_id) {
            _nearby_badges[i].timeout_in_sec = 60;
            return;
        }
    }
    // New badge \o/
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec == 0) {
            _nearby_badges[i].id = badge_id;
            _nearby_badges[i].timeout_in_sec = 60;
            break;
        }
    }
}

static void nsec_nearby_update_led_pettern(void) {
    uint32_t delay;
    uint8_t count = nsec_nearby_badges_current_count();
    if(count < 5) {
        delay = 700 - (100 * count);
    }
    else {
        delay = 300 - (50 * (count - 4));
    }
    nsec_led_set_delay(delay);
}

uint8_t nsec_nearby_badges_current_count(void) {
    uint8_t count = 0;
    for(uint8_t i = 0; i < NSEC_MAX_NEARBY_BADGES_COUNT; i++) {
        if(_nearby_badges[i].timeout_in_sec > 0) {
            count++;
        }
    }
    return count;
}
