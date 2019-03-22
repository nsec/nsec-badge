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
#include <stdbool.h>

// Includes from Nordic.
#include <crc32.h>
#include <app_util_platform.h>

// Includes from our code.
#include <drivers/flash.h>

#define PERSISTENCY_BASE_ADDRESS 0x07F000
#define PERSISTENCY_SIZE 4096

/* TODO migrate nsec_storage here */

struct persistency {
    uint32_t zombie_odds_modifier;
    // Add leds
    // Add password
    // Add identity
    // Add Other things
    uint8_t padding[4088]; // 4k - 4 (used memory) - CRC
    uint32_t crc;
};

static uint8_t persistency_bin[4096];
static struct persistency *persistency = (struct persistency*)persistency_bin;
static bool is_loaded = false;

static void set_default_persistency(void)
{
    memset(persistency_bin, 0, 4096);

    // Add here default config for your data
    persistency->zombie_odds_modifier = 0;

    persistency->crc = crc32_compute(persistency_bin, 4092, NULL);
}

static void update_persistency(void)
{
    ret_code_t ret = flash_erase(PERSISTENCY_BASE_ADDRESS);
    APP_ERROR_CHECK(ret);

    for (int i = 0; i < PERSISTENCY_SIZE/128; i++) {
        int offset = i * 128;
        ret = flash_write_128(PERSISTENCY_BASE_ADDRESS + offset,
            persistency_bin + offset);
        APP_ERROR_CHECK(ret);
    }
}

uint32_t get_persist_zombie_odds_modifier(void)
{
    return persistency->zombie_odds_modifier;
}

void set_persist_zombie_odds_modifier(uint32_t odds)
{
    persistency->zombie_odds_modifier = odds;
    update_persistency();
}

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
        update_persistency();
        is_loaded = true;
        return;
    }

    uint32_t c_crc = crc32_compute(persistency_bin, 4092, NULL);

    if (c_crc != persistency->crc) {
        set_default_persistency();
        update_persistency();
    }

    is_loaded = true;

    return;

}