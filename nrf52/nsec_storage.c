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
#include "ws2812fx.h"
#include "nsec_led_settings.h"
#include "power.h"

#define PAGE_START_MAGIC    0xDEADC0DE

typedef struct led_settings_t {
	uint8_t mode;
	uint16_t speed;
	uint8_t brightness;
	uint32_t colors[NUM_COLORS];
} Led_settings;

Led_settings default_settings = {FX_MODE_STATIC, MEDIUM_SPEED, MEDIUM_BRIGHTNESS, 
								{RED, GREEN, BLUE}};
Led_settings actual_settings;
bool need_led_settings_update = false;

bool isInit = false;

NRF_FSTORAGE_DEF(nrf_fstorage_t fs_led_settings) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = NULL,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
    .start_addr = 0x68000,
    .end_addr   = 0x68FFF,
};

void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
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
}

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
	if (!isInit) {
		nsec_storage_init();
	}
	setBrightness_WS2812FX(actual_settings.brightness);
	setMode_WS2812FX(actual_settings.mode);
	setSpeed_WS2812FX(actual_settings.speed);
	setArrayColor_packed_WS2812FX(actual_settings.colors[0], 0);
	setArrayColor_packed_WS2812FX(actual_settings.colors[1], 1);
	setArrayColor_packed_WS2812FX(actual_settings.colors[2], 2);
}

void nsec_storage_init() {
    ret_code_t rc;

    if (isInit) {
    	return;
    }

    nrf_fstorage_api_t *p_fs_api;
    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fs_led_settings, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);
    
    /* New device check */
    uint32_t new_dev_memory;
    rc = nrf_fstorage_read(&fs_led_settings, fs_led_settings.start_addr, &new_dev_memory, 4);
    APP_ERROR_CHECK(rc);
    wait_for_flash_ready(&fs_led_settings);

    if (new_dev_memory == PAGE_START_MAGIC) {
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

    isInit = true;
}
