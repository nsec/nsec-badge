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

#ifndef PERSISTENCY_H
#define PERSISTENCY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void load_persistency(void);

uint32_t get_persist_zombie_odds_modifier(void);
void set_persist_zombie_odds_modifier(uint32_t odds);

uint8_t get_stored_display_brightness(void);
void update_stored_display_brightness(uint8_t brightness);

void load_led_settings(void);
void update_stored_brightness(uint8_t brightness);
void update_stored_mode(uint8_t segment_index, uint8_t mode);
void update_stored_speed(uint8_t segment_index, uint16_t speed);
void update_stored_color(uint8_t segment_index, uint32_t color, uint8_t index);
void update_stored_reverse(uint8_t segment_index, bool reverse);
void update_stored_control(bool control);
void load_stored_led_default_settings(void);
#endif
