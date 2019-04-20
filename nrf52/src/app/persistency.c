/*
 * Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
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

#include "persistency.h"

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes from Nordic.
#include <crc32.h>
#include <app_util_platform.h>

// Includes from our code.
#include <drivers/display.h>
#include <drivers/flash.h>
#include <drivers/ws2812fx.h>
#include "nsec_led_settings.h"

#define PERSISTENCY_BASE_ADDRESS 0x07F000
#define PERSISTENCY_SIZE 4096

/* TODO migrate nsec_storage here */

/* Led settings  19 bytes*/
struct led_settings {
    uint32_t colors[NUM_COLORS];
    uint16_t speed;
    uint8_t brightness;
    uint8_t mode;
    bool reverse;
    bool control;
}__attribute__((packed));

struct persistency {
    uint32_t zombie_odds_modifier;      // 4 bytes
    uint8_t display_brightness;         // 1 bytes
    struct led_settings led_settings;   // 18 bytes
    // Add password
    // Add identity
    // Add Other things
    uint8_t padding[4096 - 23 - 4]; // 4k - (used memory) - CRC
    uint32_t crc; // 4 bytes
}__attribute__((packed));

static uint8_t persistency_bin[4096];
static struct persistency *persistency = (struct persistency*)persistency_bin;
static bool is_loaded = false;

static void update_persistency(void)
{
    persistency->crc = crc32_compute(persistency_bin, 4092, NULL);

    ret_code_t ret = flash_erase(PERSISTENCY_BASE_ADDRESS);
    APP_ERROR_CHECK(ret);

    for (int i = 0; i < PERSISTENCY_SIZE/128; i++) {
        int offset = i * 128;
        ret = flash_write_128(PERSISTENCY_BASE_ADDRESS + offset,
            persistency_bin + offset);
        APP_ERROR_CHECK(ret);
    }
}

static void set_default_led_settings(void)
{
    persistency->led_settings.mode = FX_MODE_STATIC;
    persistency->led_settings.speed = MEDIUM_SPEED;
    persistency->led_settings.brightness = MEDIUM_BRIGHTNESS;
    persistency->led_settings.colors[0] = BLUE;
    persistency->led_settings.colors[1] = RED;
    persistency->led_settings.colors[2] = GREEN;
    persistency->led_settings.reverse = false;
    persistency->led_settings.control = true;
}

static void set_default_persistency(void)
{
    memset(persistency_bin, 0, 4096);

    // Add here default config for your data
    persistency->zombie_odds_modifier = 0;
    persistency->display_brightness = 50;

    set_default_led_settings();

    update_persistency();
}

/* MODE ZOMBIE */
uint32_t get_persist_zombie_odds_modifier(void)
{
    return persistency->zombie_odds_modifier;
}

void set_persist_zombie_odds_modifier(uint32_t odds)
{
    persistency->zombie_odds_modifier = odds;
    update_persistency();
}

/* DISPLAY SETTINGS */
uint8_t get_stored_display_brightness(void) {
    return persistency->display_brightness;
}

void update_stored_display_brightness(uint8_t brightness) {
    persistency->display_brightness = brightness;
    update_persistency();
}

/* LED SETTINGS */
void update_stored_brightness(uint8_t brightness) {
    persistency->led_settings.brightness = brightness;
    update_persistency();
}

void update_stored_mode(uint8_t mode) {
    persistency->led_settings.mode = mode;
    update_persistency();
}

void update_stored_speed(uint16_t speed) {
    persistency->led_settings.speed = speed;
    update_persistency();
}

void update_stored_color(uint32_t color, uint8_t index) {
    if (index < NUM_COLORS) {
        persistency->led_settings.colors[index] = color;
        update_persistency();
    }
}

void update_stored_reverse(bool reverse) {
    persistency->led_settings.reverse = reverse;
   update_persistency();
}

void update_stored_control(bool control) {
    persistency->led_settings.control = control;
    update_persistency();
}

void load_led_settings(void) {
    if (persistency->led_settings.control) {
        start_WS2812FX();
    } else {
        stop_WS2812FX();
    }

    resetSegments_WS2812FX();

    setBrightness_WS2812FX(persistency->led_settings.brightness);
    setMode_WS2812FX(persistency->led_settings.mode);
    setSpeed_WS2812FX(persistency->led_settings.speed);
    setArrayColor_packed_WS2812FX(persistency->led_settings.colors[0], 0);
    setArrayColor_packed_WS2812FX(persistency->led_settings.colors[1], 1);
    setArrayColor_packed_WS2812FX(persistency->led_settings.colors[2], 2);
    setReverse_WS2812FX(persistency->led_settings.reverse);
}

void load_stored_led_default_settings(void) {
    set_default_led_settings();
    load_led_settings();
    update_persistency();
}

#ifndef SOLDERING_TRACK
void load_persistency(void) {
    uint8_t data[128];
    ret_code_t ret;

    memset(persistency_bin, 0, sizeof(struct persistency));

    if (is_loaded) {
        return;
    }

    for (int i = 0; i < PERSISTENCY_SIZE/128; i++) {
        int offset = i * 128;
        memset(data, 0, 128);
        ret = flash_read_128(PERSISTENCY_BASE_ADDRESS + offset, data);
        APP_ERROR_CHECK(ret);

        memcpy(persistency_bin + offset, data, 128);
    }

    if (persistency->crc == 0xFFFFFFFF) {
        // *probably* an empty persistency
        set_default_persistency();
        is_loaded = true;
        return;
    }

    uint32_t c_crc = crc32_compute(persistency_bin, 4092, NULL);
    if (c_crc != persistency->crc) {
        set_default_persistency();
    }

    load_led_settings();
    display_set_brightness(persistency->display_brightness);

    is_loaded = true;

    return;
}
#else
void load_persistency(void)
{
}
#endif
