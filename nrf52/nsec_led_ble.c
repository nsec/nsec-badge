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

#include <nrf.h>
#include <nordic_common.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ble/nsec_ble.h"
#include "ble/service_characteristic.h"
#include "ble/ble_device.h"
#include "app_glue.h"
#include "ws2812fx.h"
#include "nsec_storage.h"

VendorService led_service;
ServiceCharacteristic segment_index, first_led, last_led, pattern,
						color, speed, reverse, apply, active, bright;

bool is_ble_controled = false;

typedef struct segment_ble {
	bool	 active;
	uint8_t  index;
    uint16_t start;
    uint16_t stop;
    uint16_t speed;
    uint8_t  mode;
    bool     reverse;
    uint32_t colors[NUM_COLORS];
} SegmentBle;

// Possible to set up to 8 segment
uint8_t selected_segment = 0;
SegmentBle segment_array[8];
uint8_t brightness = 60;

void nsec_led_ble_init(void) {

	for (int i = 0; i < ARRAY_SIZE(segment_array); i++) {
		segment_array[i].active = false;
		segment_array[i].index = i;
		segment_array[i].start = 0;
		segment_array[i].stop = 0;
		segment_array[i].speed = 1000;
		segment_array[i].mode = FX_MODE_STATIC;
		segment_array[i].reverse = false;
		segment_array[i].colors[0] = RED;
		segment_array[i].colors[1] = GREEN;
		segment_array[i].colors[2] = BLUE;
	}

	uint8_t buffer[12] = {0};

	create_vendor_service(&led_service);
    add_vendor_service(&led_service);

	add_characteristic_to_vendor_service(&led_service, &segment_index, 1, AUTO_READ, AUTO_WRITE);
    set_characteristic_value(&segment_index, &segment_array[selected_segment].index);

	add_characteristic_to_vendor_service(&led_service, &first_led, 2, AUTO_READ, AUTO_WRITE);
	buffer[0] = (uint8_t) (segment_array[selected_segment].start); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].start >> 8);
	set_characteristic_value(&first_led, buffer);

	add_characteristic_to_vendor_service(&led_service, &last_led, 2, AUTO_READ, AUTO_WRITE);
	buffer[0] = (uint8_t) (segment_array[selected_segment].stop); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].stop >> 8);
	set_characteristic_value(&last_led, buffer);

	add_characteristic_to_vendor_service(&led_service, &pattern, 1, AUTO_READ, AUTO_WRITE);
	set_characteristic_value(&pattern, &segment_array[selected_segment].mode);

	add_characteristic_to_vendor_service(&led_service, &color, 12, AUTO_READ, AUTO_WRITE);
	memcpy(buffer, &segment_array[selected_segment].colors[0], 4);
	memcpy(buffer + 4, &segment_array[selected_segment].colors[1], 4);
	memcpy(buffer + 8, &segment_array[selected_segment].colors[2], 4);
	set_characteristic_value(&color, buffer);

	add_characteristic_to_vendor_service(&led_service, &speed, 2, AUTO_READ, AUTO_WRITE);
	buffer[0] = (uint8_t) (segment_array[selected_segment].speed); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].speed >> 8);
	set_characteristic_value(&speed, buffer);

	add_characteristic_to_vendor_service(&led_service, &reverse, 1, AUTO_READ, AUTO_WRITE);
	set_characteristic_value(&reverse,(uint8_t*)&segment_array[selected_segment].reverse);

	add_characteristic_to_vendor_service(&led_service, &active, 1, AUTO_READ, AUTO_WRITE);
	set_characteristic_value(&active, (uint8_t*)&segment_array[selected_segment].active);

	add_characteristic_to_vendor_service(&led_service, &bright, 1, AUTO_READ, AUTO_WRITE);
	set_characteristic_value(&bright, &brightness);

	//This will probably be removed
	set_default_advertised_service(&led_service);
}

//Control via the menu and via ble ?
void ble_take_over(void) {
	if (is_ble_controled) {
		is_ble_controled = true;
		resetSegments_WS2812FX();
	}
}

void set_selected_segment(uint8_t select) {
	selected_segment = select;
}

void update_segment_active() {
	if (segment_array[selected_segment].active) {
		ble_take_over();

		setSegment_color_array_WS2812FX(segment_array[selected_segment].index,
			segment_array[selected_segment].start,
			segment_array[selected_segment].stop,
			segment_array[selected_segment].mode, 
			segment_array[selected_segment].colors,
			segment_array[selected_segment].speed,
			segment_array[selected_segment].reverse);
	} else {
		//We cannot turn off a segment... but we can make it black and static !
		setSegment_WS2812FX(segment_array[selected_segment].index,
			segment_array[selected_segment].start,
			segment_array[selected_segment].stop,
			FX_MODE_STATIC,
			BLACK, 
			segment_array[selected_segment].speed,
			segment_array[selected_segment].reverse);
	}
}

void set_segment_active(bool active) {
	segment_array[selected_segment].active = active;
}

void set_segment_start(uint16_t start) {
	segment_array[selected_segment].start = start;
}

void set_segment_stop(uint16_t stop) {
	segment_array[selected_segment].stop = stop;
}

void set_segment_mode(uint8_t mode) {
	segment_array[selected_segment].mode = mode;
}

void set_segment_speed(uint16_t speed) {
	segment_array[selected_segment].speed = speed;
}

void set_segment_colors(uint32_t first, uint32_t second, uint32_t third) {
	segment_array[selected_segment].colors[0] = first;
	segment_array[selected_segment].colors[1] = second;
	segment_array[selected_segment].colors[2] = third;
}

void set_segment_reverse(bool reverse) {
	segment_array[selected_segment].reverse = reverse;
}

void set_brightness(uint8_t new_brightness) {
	brightness = new_brightness;
}

