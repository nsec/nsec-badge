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
#define PERSISTENCY_REVISION 0 // never match

struct led_settings {
    segment segment[NEOPIXEL_COUNT];
    uint8_t num_segment;
    bool control;
    uint8_t brightness;
}__attribute__((packed));

struct persistency {
    uint32_t zombie_odds_modifier;      // 4 bytes
    uint8_t display_brightness;         // 1 byte
    struct led_settings led_settings;   // 303 bytes
    char identity_name[17];             // 17 bytes
    uint32_t unlocked_pattern_bf;       // 4 byte
    uint8_t display_model;              // 1 byte
    uint8_t screensaver;                // 1 byte
    uint8_t ble_enable;                 // 1 byte
//    uint8_t padding[4096 - 332 - 5];    // 4k - (used memory) - rev - CRC
    uint8_t revision;
    uint32_t crc; // 4 bytes
}__attribute__((packed));

// Static assert to make sure the size of struct persistency is as expected.
//static int persistency_size_static[(sizeof(struct persistency) == 4096) ? 1 : -1] __attribute__((unused));

static uint8_t persistency_bin[4096];
static struct persistency *persistency = (struct persistency*)persistency_bin;
static bool is_loaded = false;

void update_persistency(void)
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
    persistency->led_settings.brightness = LOW_BRIGHTNESS;
    persistency->led_settings.control = true;
    persistency->led_settings.num_segment = 1;

    for (int i = 0 ; i < NEOPIXEL_COUNT; i++) {
        persistency->led_settings.segment[i].start = 0;
        persistency->led_settings.segment[i].stop = NEOPIXEL_COUNT - 1;
        persistency->led_settings.segment[i].mode = FX_MODE_SINGLE_DYNAMIC;
        persistency->led_settings.segment[i].speed = MEDIUM_SPEED;
        persistency->led_settings.segment[i].colors[0] = BLUE;
        persistency->led_settings.segment[i].colors[1] = RED;
        persistency->led_settings.segment[i].colors[2] = GREEN;
        persistency->led_settings.segment[i].reverse = false;
    }
}

void set_default_persistency(void)
{
    memset(persistency_bin, 0, 4096);

    // Add here default config for your data
    persistency->zombie_odds_modifier = 0;
    persistency->display_brightness = 50;
    persistency->display_model = 0;
    persistency->unlocked_pattern_bf = 0;
    persistency->ble_enable = true;
    persistency->revision = PERSISTENCY_REVISION;
    persistency->screensaver = 2;

    snprintf(persistency->identity_name, 16, "Citizen #%02ld",
                 (NRF_FICR->DEVICEID[0] & 0xFFFF));

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

uint8_t get_stored_display_model(void) {
    return persistency->display_model;
}

void update_stored_display_model(uint8_t model) {
    persistency->display_model = model;
    update_persistency();
}

uint8_t get_stored_screensaver(void) {
    return persistency->screensaver;
}

void update_stored_screensaver(uint8_t mode) {
    persistency->screensaver = mode;
    update_persistency();
}

/* LED SETTINGS */
void update_stored_num_segment(uint8_t num_segment, bool update)
{
    persistency->led_settings.num_segment = num_segment;
    if (update) {
        update_persistency();
    }
}

void update_stored_segment(uint8_t segment_index, uint16_t start, uint16_t stop,
                           uint8_t mode, uint32_t color_1, uint32_t color_2,
                           uint32_t color_3, uint16_t speed, bool reverse,
                           bool update)
{
    persistency->led_settings.segment[segment_index].start = start;
    persistency->led_settings.segment[segment_index].stop = stop;
    persistency->led_settings.segment[segment_index].mode = mode;
    persistency->led_settings.segment[segment_index].colors[0] = color_1;
    persistency->led_settings.segment[segment_index].colors[1] = color_2;
    persistency->led_settings.segment[segment_index].colors[2] = color_3;
    persistency->led_settings.segment[segment_index].speed = speed;
    persistency->led_settings.segment[segment_index].reverse = reverse;
    if (update) {
        update_persistency();
    }
}

void move_stored_segment(uint8_t src, uint8_t dest, bool update)
{
    persistency->led_settings.segment[src] =
        persistency->led_settings.segment[dest];
    if (update) {
        update_persistency();
    }
}

void update_stored_start(uint8_t segment_index, uint16_t start, bool update)
{
    persistency->led_settings.segment[segment_index].start = start;
    if (update) {
        update_persistency();
    }
}

void update_stored_stop(uint8_t segment_index, uint16_t stop, bool update)
{
    persistency->led_settings.segment[segment_index].stop = stop;
    if (update) {
        update_persistency();
    }
}

void update_stored_brightness(uint8_t brightness, bool update) {
    persistency->led_settings.brightness = brightness;
    if (update) {
        update_persistency();
    }
}

void update_stored_mode(uint8_t segment_index, uint8_t mode, bool update) {
    persistency->led_settings.segment[segment_index].mode = mode;
    if (update) {
        update_persistency();
    }
}

void update_stored_speed(uint8_t segment_index, uint16_t speed, bool update) {
    persistency->led_settings.segment[segment_index].speed = speed;
    if (update) {
        update_persistency();
    }
}

void update_stored_color(uint8_t segment_index, uint32_t color, uint8_t index, bool update) {
    if (index < NUM_COLORS) {
        persistency->led_settings.segment[segment_index].colors[index] = color;
        if (update) {
            update_persistency();
        }
    }
}

void update_stored_reverse(uint8_t segment_index, bool reverse, bool update) {
    persistency->led_settings.segment[segment_index].reverse = reverse;
    if (update) {
        update_persistency();
    }
}

void update_stored_control(bool control, bool update) {
    persistency->led_settings.control = control;
    if (update) {
        update_persistency();
    }
}

void load_led_settings(void) {
    if (persistency->led_settings.control) {
        start_WS2812FX();
    } else {
        stop_WS2812FX();
    }

    resetSegments_WS2812FX();
    setNumSegments_WS2812FX(persistency->led_settings.num_segment);

    // Avoid booting with a closed display...
    if (persistency->led_settings.brightness == 0) {
        setBrightness_WS2812FX(1);
    } else {
        setBrightness_WS2812FX(persistency->led_settings.brightness);
    }

    for (int i = 0; i < persistency->led_settings.num_segment; i++) {
        setSegmentStart_WS2812FX(i, persistency->led_settings.segment[i].start);
        setSegmentStop_WS2812FX(i, persistency->led_settings.segment[i].stop);
        setSegmentMode_WS2812FX(i, persistency->led_settings.segment[i].mode);
        setSegmentSpeed_WS2812FX(i, persistency->led_settings.segment[i].speed);
        setSegmentArrayColor_packed_WS2812FX(
            i, 0, persistency->led_settings.segment[i].colors[0]);
        setSegmentArrayColor_packed_WS2812FX(
            i, 1, persistency->led_settings.segment[i].colors[1]);
        setSegmentArrayColor_packed_WS2812FX(
            i, 2, persistency->led_settings.segment[i].colors[2]);
        setSegmentReverse_WS2812FX(
            i, persistency->led_settings.segment[i].reverse);
    }
}

void load_stored_led_default_settings(void) {
    set_default_led_settings();
    load_led_settings();
    update_persistency();
}

/* IDENTITY */
void update_identity(char *new_identity) {
    memset(persistency->identity_name, 0, 16);
    strncpy(persistency->identity_name, new_identity, 16);
    persistency->identity_name[16] = '\0';
    update_persistency();
}

void load_stored_identity(char *identity) {
    memset(identity, 0, 16);
    strncpy(identity, persistency->identity_name, 16);
}

char* get_stored_identity(void) {
    return persistency->identity_name;
}

/* PATTERN */
uint32_t get_stored_pattern_bf(void) {
    return persistency->unlocked_pattern_bf;
}

void update_stored_pattern_bf(uint32_t bf) {
    persistency->unlocked_pattern_bf = bf;
    update_persistency();
}

/* BLE */
void update_stored_ble_is_enabled(bool nsec_ble_is_enabled)
{
    persistency->ble_enable = nsec_ble_is_enabled;
    update_persistency();
}

bool get_stored_ble_is_enabled(void)
{
    return persistency->ble_enable;
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

    if (persistency->revision != PERSISTENCY_REVISION) {
        set_default_persistency();
    }

    load_led_settings();
    display_set_brightness(persistency->display_brightness);
    display_set_model(persistency->display_model);

    is_loaded = true;

    return;
}
#else
void load_persistency(void)
{
}
#endif
