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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nrf.h"
#include <nrf_delay.h>
#include "nordic_common.h"
#include "boards.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"

#include "nsec_storage.h"
#include "led_code_storage.h"
#include "ws2812fx.h"
#include "nsec_led_settings.h"
#include "power.h"

#define PAGE_START_MAGIC    0xDEADC0DE

/* Led settings */
typedef struct LedSettings_t {
	uint8_t mode;
	uint16_t speed;
	uint8_t brightness;
	uint32_t colors[NUM_COLORS];
} LedSettings;

LedSettings default_settings = {FX_MODE_STATIC, MEDIUM_SPEED, MEDIUM_BRIGHTNESS, 
								{RED, GREEN, BLUE}};
LedSettings actual_settings;
bool need_led_settings_update = false;

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_led_settings) =
{
    .evt_handler = NULL,
    .start_addr = 0x68000,
    .end_addr   = 0x68FFF,
};

/* password */
uint32_t stored_password;

uint32_t pw[] = {SPONSOR_0_PW, SPONSOR_1_PW, SPONSOR_2_PW, SPONSOR_3_PW, SPONSOR_4_PW,
                SPONSOR_5_PW, SPONSOR_6_PW, SPONSOR_7_PW, SPONSOR_8_PW, SPONSOR_9_PW,
                SPONSOR_10_PW, SPONSOR_11_PW, SPONSOR_12_PW, SPONSOR_13_PW, SPONSOR_14_PW,
                SPONSOR_15_PW};

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_password) =
{
    .evt_handler = NULL,
    .start_addr = 0x69000,
    .end_addr   = 0x69FFF,
};
bool need_password_update = false;

bool is_init = false;

static void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy*/
    while (nrf_fstorage_is_busy(p_fstorage)) {
        power_manage();
    }
}

void nsec_storage_update() {
    ret_code_t rc;

    if (need_led_settings_update) {
        rc = nrf_fstorage_erase(&fs_led_settings, fs_led_settings.start_addr, 1, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_led_settings);

        rc = nrf_fstorage_write(&fs_led_settings, fs_led_settings.start_addr, &actual_settings,
        						sizeof(actual_settings), NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_led_settings);
        need_led_settings_update = false;
    }

    if (need_password_update) {
        rc = nrf_fstorage_erase(&fs_password, fs_password.start_addr, 1, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_password);

        rc = nrf_fstorage_write(&fs_password, fs_password.start_addr, &stored_password, 4, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_password);
        need_password_update = false;
    }
}

/* Led Settings interface */
void update_stored_brightness(uint8_t brightness) {
	actual_settings.brightness = brightness;
    need_led_settings_update = true;
}

void update_stored_mode(uint8_t mode) {
	actual_settings.mode = mode;
	need_led_settings_update = true;
}

void update_stored_speed(uint16_t speed) {
	actual_settings.speed = speed;
	need_led_settings_update = true;
}

void update_stored_color(uint32_t color, uint8_t index) {
	if (index < NUM_COLORS) {
		actual_settings.colors[index] = color;
		need_led_settings_update = true;
	}
}

void load_stored_led_settings(void) {
	if (!is_init) {
		nsec_storage_init();
	}
	setBrightness_WS2812FX(actual_settings.brightness);
	setMode_WS2812FX(actual_settings.mode);
	setSpeed_WS2812FX(actual_settings.speed);
	setArrayColor_packed_WS2812FX(actual_settings.colors[0], 0);
	setArrayColor_packed_WS2812FX(actual_settings.colors[1], 1);
	setArrayColor_packed_WS2812FX(actual_settings.colors[2], 2);
}

void load_stored_led_default_settings(void) {
     memcpy(&actual_settings, &default_settings, sizeof(LedSettings));
     need_led_settings_update = true;
     load_stored_led_settings();
}

/* code interface */

static void unlock_all_pattern() {
    if (stored_password != 0xFFFFFFFF) {
        stored_password = 0xFFFFFFFF;
        need_password_update = true;
    }
}

static void unlock_pattern(uint32_t sponsor_index) {
    if (sponsor_index < 31) {
        SET_BIT(stored_password, sponsor_index);
        need_password_update = true;
    }
}

uint32_t nsec_get_pattern_pw(uint32_t sponsor_index) {
    if (sponsor_index < SPONSOR_PW_SIZE) {
        return pw[sponsor_index];
    }
    return 0;
}

bool pattern_is_unlock(uint32_t sponsor_index) {
    return IS_SET(stored_password, sponsor_index);
}

// true valid, false invalid
bool nsec_unlock_led_pattern(uint32_t password) {
    if (password == MASTER_PW) {
        unlock_all_pattern();
        return true;
    }
    for (int i = 0; i < SPONSOR_PW_SIZE; i++) {
        if (password == pw[i]) {
            if (!pattern_is_unlock(i)) {
                unlock_pattern(i);
            }
            return true;
        }
    }
    return false;
}

static bool is_new_memory_page(nrf_fstorage_t const * p_fstorage) {
    ret_code_t rc;
    uint32_t new_dev_memory;
    rc = nrf_fstorage_read(&fs_led_settings, fs_led_settings.start_addr, &new_dev_memory, 4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_led_settings);

    return (new_dev_memory == PAGE_START_MAGIC) ? true : false;
} 

static void led_settings_storage_init(void) {
    ret_code_t rc;

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fs_led_settings, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
    
    if (is_new_memory_page(&fs_led_settings)) {
        //Store the default settings
        rc = nrf_fstorage_write(&fs_led_settings, fs_led_settings.start_addr, &default_settings, 
                                sizeof(actual_settings), NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_led_settings);
    }

    //Load actual settings
    rc = nrf_fstorage_read(&fs_led_settings, fs_led_settings.start_addr, &actual_settings,
                            sizeof(actual_settings));
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_led_settings);
}

static void password_storage_init(void) {
    ret_code_t rc;

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fs_password, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
    
    if (is_new_memory_page(&fs_password)) {
        //Store the default settings
        rc = nrf_fstorage_write(&fs_password, fs_password.start_addr, &stored_password, 4, NULL);
        APP_ERROR_CHECK(rc);
        wait_for_flash_ready(&fs_led_settings);
    }

    //Load actual settings
    rc = nrf_fstorage_read(&fs_password, fs_password.start_addr, &stored_password, 4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_led_settings);
}

void nsec_storage_init(void) {
   
    if (is_init) {
    	return;
    }

    led_settings_storage_init();
    password_storage_init();

    is_init = true;
}
