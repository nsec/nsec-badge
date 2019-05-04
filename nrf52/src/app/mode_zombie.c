//  Copyright (c) 2019
//  Eric Tremblay <habscup@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <app_timer.h>
#include <string.h>
#include "cli.h"
#include <nrf_delay.h>
#include <ble/uuid.h>

#include "drivers/display.h"
#include "drivers/ws2812fx.h"
#include "drivers/controls.h"

#include "ble/ble_device.h"
#include "ble/service_characteristic.h"
#include "ble/vendor_service.h"

#include "application.h"
#include "home_menu.h"
#include "mode_zombie.h"
#include "persistency.h"
#include "timer.h"

// Until we get the bonding... this is how a service is protected
#define MAGIC_ARRAY_SIZE 25
static uint8_t magic_array[MAGIC_ARRAY_SIZE] = {
    0x92, 0x1a, 0xcf, 0x1,  0x9,  0x23, 0x7e, 0x42, 0xe0,
    0x6,  0x10, 0x43, 0x4b, 0x9e, 0x79, 0x45, 0x4a, 0xe6,
    0xaa, 0x46, 0xdb, 0xfc, 0x3e, 0x5d, 0xa1};

static struct VendorService zombie_ble_service;
static struct ServiceCharacteristic trigger_characteristic;

static uint16_t service_uuid = 0x2D00;
static uint16_t trigger_uuid = 0x2D01;

APP_TIMER_DEF(m_zombie_timer_id);
static bool process_mode_zombie = false;

/*
 * Callback function
 */
static void mode_zombie_timer_handler(void *p_context) {
    process_mode_zombie = true;
}

void app_mode_zombie(void (*service_device)()) {
    uint64_t start = get_current_time_millis();
    uint64_t elapse = 0;

    // Make sure Screen is on !
    display_set_brightness(100);

    /* Set the neopixel pattern */
    resetSegments_WS2812FX();
    setMode_WS2812FX(FX_MODE_BLINK);
    setArrayColor_packed_WS2812FX(RED, 0);
    setArrayColor_packed_WS2812FX(ORANGE, 1);
    setSpeed_WS2812FX(100);

    while (elapse <= ZOMBIE_MODE_DURATION) {
        for (int i=0; i < 32; i++) {
            uint8_t x = nsec_random_get_byte(160);
            uint8_t y = nsec_random_get_byte(80);
            uint16_t color = nsec_random_get_u16(0xFFFF);
            display_draw_pixel(x, y, color);
	}
	/* Service device and wait for next interrupt */
        service_device();
        elapse = get_current_time_millis() - start;
    }

    /* Restore badge normal state */
    load_led_settings();

    /* Re-enable buttons */
    nsec_controls_enable();

    /* Return to default app */
    application_clear();
}

void mode_zombie_process(void) {
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
        nsec_controls_disable();

	/* Set zombie as the next app to run if it's not already */
	if (application_get() != app_mode_zombie) {
            application_set(app_mode_zombie);
	}

        // Reset the odds
        set_persist_zombie_odds_modifier(0);
    }
}

static uint16_t on_trigger(CharacteristicWriteEvent* event)
{
    if (!memcmp(event->data_buffer, &magic_array, event->data_length)) {
        if (application_get() != app_mode_zombie) {
            application_set(app_mode_zombie);
        }
        return BLE_GATT_STATUS_SUCCESS;
    } else {
        nrf_delay_ms(1000);
        return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
    }
}

/*
 * Initialize the app timer library and heartbeat
 */
void mode_zombie_init(void) {
    ret_code_t err_code;

    err_code = app_timer_create(&m_zombie_timer_id, APP_TIMER_MODE_REPEATED,
                                mode_zombie_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_zombie_timer_id,
                               APP_TIMER_TICKS(ZOMBIE_TIMER_TIMEOUT), NULL);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t uuid = {.uuid = service_uuid, .type = TYPE_NSEC_UUID};
    create_vendor_service(&zombie_ble_service, &uuid);
    add_vendor_service(&zombie_ble_service);

    create_characteristic(&trigger_characteristic, MAGIC_ARRAY_SIZE, DENY_READ, AUTH_WRITE_REQUEST, trigger_uuid);
    add_characteristic_to_vendor_service(&zombie_ble_service, &trigger_characteristic);
    add_write_request_handler(&trigger_characteristic, on_trigger);
}
