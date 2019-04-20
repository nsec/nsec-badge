/*
 * Copyright 2018 Eric Tremblay <habscup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "boards.h"
#include "nordic_common.h"
#include "nrf.h"
#include <nrf_delay.h>

#include "nrf_fstorage_sd.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"

#include "app/gfx_effect.h"
#include "app/identity.h"
#include "app/nsec_led_settings.h"
#include "display.h"
#include "led_code_storage.h"
#include "nsec_storage.h"
#include "power.h"
#include "ws2812fx.h"

#define PAGE_START_MAGIC 0xDEADC0DE

extern bool is_at_home_menu;

/* password */
uint32_t stored_password = 0;

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_password) = {
    .evt_handler = NULL,
    .start_addr = 0x69000,
    .end_addr = 0x69FFF,
};
bool need_password_update = false;

/* identity */
char identity_name[17];

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_identity) = {
    .evt_handler = NULL,
    .start_addr = 0x70000,
    .end_addr = 0x70FFF,
};

bool need_identity_update = false;

bool is_init = false;

static void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage) {
    /* While fstorage is busy*/
    while (nrf_fstorage_is_busy(p_fstorage)) {
        power_manage();
    }
}

void nsec_storage_update() {
    ret_code_t rc;

    if (need_password_update) {
        rc = nrf_fstorage_erase(&fs_password, fs_password.start_addr, 1, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_password);

        rc = nrf_fstorage_write(&fs_password, fs_password.start_addr,
                                &stored_password, 4, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_password);
        need_password_update = false;
    }

    if (need_identity_update) {
        rc = nrf_fstorage_erase(&fs_identity, fs_identity.start_addr, 1, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_identity);

        rc = nrf_fstorage_write(&fs_identity, fs_identity.start_addr,
                                identity_name, 16, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_identity);

        if (is_at_home_menu) {
            nsec_identity_draw();
            gfx_update();
        }
        need_identity_update = false;
    }
}

/*void update_stored_segment(SegmentBle *segment, int index) {
    if (actual_settings.ble_control_permitted) {
        if (actual_settings.is_ble_controlled) {
            actual_settings.segment_array[index].active = segment->active;
            actual_settings.segment_array[index].index = segment->index;
            actual_settings.segment_array[index].start = segment->start;
            actual_settings.segment_array[index].stop = segment->stop;
            actual_settings.segment_array[index].speed = segment->speed;
            actual_settings.segment_array[index].mode = segment->mode;
            actual_settings.segment_array[index].reverse = segment->reverse;
            actual_settings.segment_array[index].colors[0] = segment->colors[0];
            actual_settings.segment_array[index].colors[1] = segment->colors[1];
            actual_settings.segment_array[index].colors[2] = segment->colors[2];

            need_led_settings_update = true;
        }
    }
}*/

/* code interface */

static void unlock_all_pattern() {
    if (stored_password != 0x00FFFFFF) {
        stored_password = 0x00FFFFFF;
        need_password_update = true;
    }
}

static void unlock_pattern(uint32_t sponsor_index) {
    if (sponsor_index < 31) {
        SET_BIT(stored_password, sponsor_index);
        need_password_update = true;
    }
}

const char *nsec_get_pattern_pw(uint32_t sponsor_index) {
    if (sponsor_index < SPONSOR_PW_SIZE) {
        return sponsor_password[sponsor_index];
    }
    return 0;
}

bool pattern_is_unlock(uint32_t sponsor_index) {
    return IS_SET(stored_password, sponsor_index);
}

// true valid, false invalid
bool nsec_unlock_led_pattern(char *password, uint8_t index) {
    if (strcmp(password, MASTER_PW) == 0) {
        unlock_all_pattern();
        return true;
    }
    if (strcmp(password, sponsor_password[index]) == 0) {
        if (!pattern_is_unlock(index)) {
            unlock_pattern(index);
        }
        return true;
    }
    return false;
}

// identity
void update_identity(char *new_identity) {
    memset(identity_name, 0, 16);
    strncpy(identity_name, new_identity, 16);
    need_identity_update = true;
}

void load_stored_identity(char *identity) {
    if (!is_init) {
        nsec_storage_init();
    }
    memset(identity, 0, 16);
    strncpy(identity, identity_name, 16);
}

static bool is_new_memory_page(nrf_fstorage_t const *p_fstorage) {
    ret_code_t rc;
    uint32_t new_dev_memory;
    rc = nrf_fstorage_read(p_fstorage, p_fstorage->start_addr, &new_dev_memory,
                           4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(p_fstorage);

    return (new_dev_memory == 0xFFFFFFFF) ? true : false;
}

static void password_storage_init(void) {
    ret_code_t rc;

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fs_password, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);

    if (is_new_memory_page(&fs_password)) {
        // Store the default settings
        rc = nrf_fstorage_write(&fs_password, fs_password.start_addr,
                                &stored_password, 4, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_password);
    }

    // Load actual settings
    rc = nrf_fstorage_read(&fs_password, fs_password.start_addr,
                           &stored_password, 4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_password);
}

static void identity_storage_init(void) {
    ret_code_t rc;

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fs_identity, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);

    if (is_new_memory_page(&fs_identity)) {
        // Store the default settings
#if defined(NSEC_HARDCODED_BADGE_IDENTITY_NAME)
#define NSEC_STRINGIFY_(...) #__VA_ARGS__
#define NSEC_STRINGIFY(...) NSEC_STRINGIFY_(__VA_ARGS__)
        snprintf(identity_name, 16,
                 NSEC_STRINGIFY(NSEC_HARDCODED_BADGE_IDENTITY_NAME));
#else
        snprintf(identity_name, 16, "Cosmonaut #%02ld",
                 (NRF_FICR->DEVICEID[0] & 0xFFFF));
#endif
        rc = nrf_fstorage_write(&fs_identity, fs_identity.start_addr,
                                identity_name, 16, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_identity);
    }

    // Load actual settings
    rc = nrf_fstorage_read(&fs_identity, fs_identity.start_addr, identity_name,
                           16);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_identity);
}

void nsec_storage_init(void) {

    if (is_init) {
        return;
    }

    password_storage_init();
    identity_storage_init();

    is_init = true;
}
