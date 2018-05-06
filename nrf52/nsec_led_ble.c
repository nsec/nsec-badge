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
#include "ble/gap_configuration.h"
#include "ble/ble_device.h"
#include "app_glue.h"
#include "ws2812fx.h"
#include "nsec_storage.h"
#include "identity.h"

uint16_t set_segment_index_char_callback(CharacteristicWriteEvent *event);
uint16_t set_start_segment_char_callback(CharacteristicWriteEvent *event);
uint16_t set_stop_segment_char_callback(CharacteristicWriteEvent *event);
uint16_t set_pattern_char_callback(CharacteristicWriteEvent *event);
uint16_t set_colors_char_callback(CharacteristicWriteEvent *event);
uint16_t set_speed_char_callback(CharacteristicWriteEvent *event);
uint16_t set_reverse_char_callback(CharacteristicWriteEvent *event);
uint16_t set_active_char_callback(CharacteristicWriteEvent *event);
uint16_t set_brightness_char_callback(CharacteristicWriteEvent *event);
uint16_t unlock_service_callback(CharacteristicWriteEvent *event);
void update_all_characteristics_value(void);
void ble_take_over(void);
void set_selected_segment(uint8_t select);
void update_segment_active_char(void);
void set_segment_active(bool active);
void set_segment_start(uint16_t start);
void set_segment_stop(uint16_t stop);
void set_segment_mode(uint8_t mode);
void set_segment_speed(uint16_t speed_char);
void set_segment_colors(uint32_t first, uint32_t second, uint32_t third);
void set_segment_reverse(bool reverse);
void set_brightness(uint8_t new_brightness);

VendorService led_service;
ServiceCharacteristic segment_index_char, start_segment_char, stop_segment_char, pattern_char,
						colors_char, speed_char, reverse_char, active_char, brightness_char,
						unlock_char, sync_char;

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
bool unlock_state = true;

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

	create_vendor_service(&led_service);
    add_vendor_service(&led_service);

	add_characteristic_to_vendor_service(&led_service, &segment_index_char, 1, AUTO_READ, REQUEST_WRITE);
    add_write_request_handler(&segment_index_char, set_segment_index_char_callback);

	add_characteristic_to_vendor_service(&led_service, &start_segment_char, 2, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&start_segment_char, set_start_segment_char_callback);

	add_characteristic_to_vendor_service(&led_service, &stop_segment_char, 2, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&stop_segment_char, set_stop_segment_char_callback);

	add_characteristic_to_vendor_service(&led_service, &pattern_char, 1, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&pattern_char, set_pattern_char_callback);

	add_characteristic_to_vendor_service(&led_service, &colors_char, 12, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&colors_char, set_colors_char_callback);

	add_characteristic_to_vendor_service(&led_service, &speed_char, 2, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&speed_char, set_speed_char_callback);

	add_characteristic_to_vendor_service(&led_service, &reverse_char, 1, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&reverse_char, set_reverse_char_callback);

	add_characteristic_to_vendor_service(&led_service, &active_char, 1, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&active_char, set_active_char_callback);

	add_characteristic_to_vendor_service(&led_service, &brightness_char, 1, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&brightness_char, set_brightness_char_callback);

	add_characteristic_to_vendor_service(&led_service, &unlock_char, 1, AUTO_READ, DENY_WRITE);
	set_characteristic_value(&unlock_char, (uint8_t *)&unlock_state);

	add_characteristic_to_vendor_service(&led_service, &sync_char, 8, AUTO_READ, REQUEST_WRITE);
	add_write_request_handler(&sync_char, unlock_service_callback);

	update_all_characteristics_value();

	//This will probably be removed
	set_default_advertised_service(&led_service);
}

void update_all_characteristics_value(void) {
	uint8_t buffer[12] = {0};
	set_characteristic_value(&segment_index_char, &segment_array[selected_segment].index);

	buffer[0] = (uint8_t) (segment_array[selected_segment].start); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].start >> 8);
	set_characteristic_value(&start_segment_char, buffer);

	buffer[0] = (uint8_t) (segment_array[selected_segment].stop); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].stop >> 8);
	set_characteristic_value(&stop_segment_char, buffer);

	set_characteristic_value(&pattern_char, &segment_array[selected_segment].mode);

	memcpy(buffer, &segment_array[selected_segment].colors[0], 4);
	memcpy(buffer + 4, &segment_array[selected_segment].colors[1], 4);
	memcpy(buffer + 8, &segment_array[selected_segment].colors[2], 4);
	set_characteristic_value(&colors_char, buffer);

	buffer[0] = (uint8_t) (segment_array[selected_segment].speed); 
 	buffer[1] = (uint8_t) (segment_array[selected_segment].speed >> 8);
	set_characteristic_value(&speed_char, buffer);

	set_characteristic_value(&reverse_char,(uint8_t*)&segment_array[selected_segment].reverse);

	set_characteristic_value(&active_char, (uint8_t*)&segment_array[selected_segment].active);

	set_characteristic_value(&brightness_char, &brightness);
}

uint16_t set_segment_index_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value = *(event->data_buffer);

	if (value >= MAX_NUM_SEGMENTS) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	set_selected_segment(value);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_start_segment_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value[2];
	memcpy(value, event->data_buffer, 2);
	uint16_t start_segment = value[0] | value[1] << 8;

	if (start_segment >= MAX_NUM_SEGMENTS) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	set_segment_start(start_segment);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_stop_segment_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value[2];
	memcpy(value, event->data_buffer, 2);
	uint16_t stop_segment = value[0] | value[1] << 8;

	if (stop_segment >= MAX_NUM_SEGMENTS) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	set_segment_stop(stop_segment);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_pattern_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value = *(event->data_buffer);

	if (value >= MODE_COUNT) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	//TODO check for unlock
	// if code unlock then denied

	set_segment_mode(value);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_colors_char_callback(CharacteristicWriteEvent *event) {
	uint8_t value[12];
	uint8_t first, second, third;
	
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	memcpy(value, event->data_buffer, 12);
	memcpy(&first, value, 4);
	memcpy(&second, value + 4, 4);
	memcpy(&third, value + 8, 4);

	set_segment_colors(first, second, third);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_speed_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value[2];
	memcpy(value, event->data_buffer, 2);
	uint16_t speed = value[0] | value[1] << 8;

	if (speed < SPEED_MIN) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	set_segment_speed(speed);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_reverse_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value = *(event->data_buffer);
	set_segment_reverse(value);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_active_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value = *(event->data_buffer);
	set_segment_active(value);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t set_brightness_char_callback(CharacteristicWriteEvent *event) {
	if (!unlock_state) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	uint8_t value = *(event->data_buffer);

	if (value > 100) {
		return BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	}

	set_brightness(value);

	return BLE_GATT_STATUS_SUCCESS;
}

uint16_t unlock_service_callback(CharacteristicWriteEvent *event) {
	uint8_t key_enterred[8];
	get_characteristic_value(&unlock_char, key_enterred);

	char key[8];
    nsec_identity_get_unlock_key(key, sizeof(key));
    if(event->data_length == strlen(key) && memcmp(key, key_enterred, event->data_length) == 0) {
    	unlock_state = true;
    } else {
    	unlock_state = false;
    }

    set_characteristic_value(&unlock_char, (uint8_t*)&unlock_state);

    return BLE_GATT_STATUS_SUCCESS;
}

//Control via the menu and via ble ?
void ble_take_over(void) {
	if (!is_ble_controled) {
		is_ble_controled = true;
		resetSegments_WS2812FX();
	}
}

void set_selected_segment(uint8_t select) {
	selected_segment = select;
	update_all_characteristics_value();
}

void update_segment_active_char(void) {
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
	update_segment_active_char();
}

void set_segment_start(uint16_t start) {
	segment_array[selected_segment].start = start;
	update_segment_active_char();
}

void set_segment_stop(uint16_t stop) {
	segment_array[selected_segment].stop = stop;
	update_segment_active_char();
}

void set_segment_mode(uint8_t mode) {
	segment_array[selected_segment].mode = mode;
	update_segment_active_char();
}

void set_segment_speed(uint16_t speed) {
	segment_array[selected_segment].speed = speed;
	update_segment_active_char();
}

void set_segment_colors(uint32_t first, uint32_t second, uint32_t third) {
	segment_array[selected_segment].colors[0] = first;
	segment_array[selected_segment].colors[1] = second;
	segment_array[selected_segment].colors[2] = third;
	update_segment_active_char();
}

void set_segment_reverse(bool reverse) {
	segment_array[selected_segment].reverse = reverse;
	update_segment_active_char();
}

void set_brightness(uint8_t new_brightness) {
	brightness = new_brightness;
	setBrightness_WS2812FX(brightness);
}

