/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * SPDX-FileCopyrightText: 2025 NorthSec
 */

#ifndef NSEC_CONFIG_HPP
#define NSEC_CONFIG_HPP

#include "scheduling/time.hpp"
#include "utils/logging.hpp"

#include <stddef.h>
#include <stdint.h>

namespace nsec::config::scheduling
{
constexpr unsigned int default_task_priority = 10;
constexpr unsigned int default_stack_size_words = 8192;
} // namespace nsec::config::scheduling

namespace nsec::config::social
{
constexpr uint8_t initial_level = 0;
constexpr uint8_t max_level = 200;
constexpr uint8_t multiple_badges_discovered_simultaneously_multiplier = 2;
} // namespace nsec::config::social

namespace nsec::config::splash
{
constexpr nsec::scheduling::relative_time_ms splash_screen_wait_time_ms = 2000;
}

namespace nsec::config::button
{
constexpr nsec::scheduling::relative_time_ms polling_period_ms = 10;

// How many ticks a button's state must be observed in to trigger.
constexpr uint8_t debounce_ticks = 2;

// Must be multiples of the polling period.
constexpr nsec::scheduling::relative_time_ms button_down_first_repeat_delay_ms =
    500;
static_assert(button_down_first_repeat_delay_ms % polling_period_ms == 0);

constexpr nsec::scheduling::relative_time_ms button_down_repeat_delay_ms = 30;
static_assert(button_down_repeat_delay_ms % polling_period_ms == 0);

} // namespace nsec::config::button

namespace nsec::config::display
{
constexpr nsec::scheduling::relative_time_ms refresh_period_ms = 16;

constexpr uint8_t menu_font_size = 1;
constexpr uint8_t scroll_font_size = 3;
constexpr uint8_t pairing_font_size = 3;

// Default font.
constexpr uint8_t font_base_width = 6;
constexpr uint8_t font_base_height = 8;

constexpr nsec::scheduling::absolute_time_ms prompt_cycle_time = 2000;

constexpr uint8_t scroll_pixels_per_second = 80;
} // namespace nsec::config::display

namespace nsec::config::communication
{
// Size reserved for protocol messages
constexpr size_t protocol_max_message_size = 16;
constexpr unsigned int badge_serial_speed = 115200;
constexpr bool invert_uart = true;
constexpr unsigned int ir_resolution_hz = 1000000;
constexpr unsigned int ir_frequency_hz = 38000;
constexpr unsigned int ir_mem_block_symbols = 48;
constexpr unsigned int ir_idle_delay_ms = 3000;
constexpr unsigned int ir_timeout_ms = 10000;
constexpr unsigned int ir_timeout_check_interval_us = 500000;
constexpr unsigned int ir_sync_request_jitter_max = 200;

/*
 * Applications may define messages >= application_message_type_range_begin.
 * IDs under this range are reserved by the wire protocol.
 */
constexpr uint8_t application_message_type_range_begin = 10;

constexpr auto rx_buffer_size = 1024;
constexpr auto tx_buffer_size = 1024;
constexpr auto uart_queue_length = 16;

constexpr nsec::scheduling::relative_time_ms network_handler_base_period_ms =
    20;
constexpr nsec::scheduling::relative_time_ms network_handler_timeout_ms = 10000;
constexpr nsec::scheduling::relative_time_ms
    network_handler_retransmit_timeout_ms = 6 * network_handler_base_period_ms;

} // namespace nsec::config::communication

namespace nsec::config::badge
{
constexpr unsigned int pairing_animation_time_per_led_progress_bar_ms = 1000;
} // namespace nsec::config::badge

namespace nsec::config::logging
{
constexpr auto uart_interface_level = nsec::logging::logger::severity::ERROR;
constexpr auto ir_interface_level = nsec::logging::logger::severity::DEBUG;
constexpr auto network_handler_level = nsec::logging::logger::severity::DEBUG;
constexpr auto button_watcher_level = nsec::logging::logger::severity::ERROR;
constexpr auto badge_level = nsec::logging::logger::severity::ERROR;
constexpr auto animator_level = nsec::logging::logger::severity::ERROR;
constexpr auto dock_detector_level = nsec::logging::logger::severity::ERROR;
} // namespace nsec::config::logging

namespace nsec::config::persistence
{
constexpr auto namespace_name = "badge2024";
constexpr auto social_level_field_name = "social_lvl";
constexpr auto social_level_check_field_name = "check_lvl";
constexpr auto selected_animation_field_name = "slctd_anim";
constexpr auto sponsor_flag_field_name = "sponsor";
constexpr auto badge_id_field_name_prefix = "b";
constexpr auto badge_paired_count_field_name = "paired_count";
} // namespace nsec::config::persistence

namespace nsec::config::i2c
{
constexpr uint8_t sponsor_cmd = 0x69;
constexpr uint8_t animation_cmd = 0x45;
}

#endif // NSEC_CONFIG_HPP
