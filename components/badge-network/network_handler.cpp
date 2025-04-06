/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#include "esp_random.h"
#include "esp_timer.h"
#include <cstdint>
#include <cstring>

#include <badge/globals.hpp>
#include <utils/board.hpp>

#include "network_handler.hpp"
#include "network_messages.hpp"

namespace nc = nsec::communication;

nc::network_handler::network_handler() noexcept
    : _ir_interface(static_cast<gpio_num_t>(nsec::board::ir::tx_pin),
                    static_cast<gpio_num_t>(nsec::board::ir::rx_pin),
                    nsec::config::communication::ir_resolution_hz,
                    nsec::config::communication::ir_frequency_hz),
      _current_ir_protocol_state{ir_protocol_state::IDLE},
      _logger("Network handler", nsec::config::logging::network_handler_level),
      _ir_timeout_timestamp(0)
{
    _reset();
    _setup();

    _ir_interface.register_packet_handler(
        [this](const message::ir_packet *packet) {
            _process_received_ir_packet(packet);
        });
}

void nc::network_handler::_set_ir_protocol_state(
    ir_protocol_state next_state) noexcept
{
    if (_current_ir_protocol_state != next_state) {
        _logger.info("IR protocol state change: {} -> {}",
                     static_cast<int>(_current_ir_protocol_state),
                     static_cast<int>(next_state));
        _current_ir_protocol_state = next_state;
    }
}

void nc::network_handler::start_ir_key_exchange() noexcept
{
    if (_current_ir_protocol_state != ir_protocol_state::IDLE) {
        _logger.info("IR exchange already in progress");
        return;
    }

    _logger.info("Starting IR key exchange");
    _set_ir_protocol_state(ir_protocol_state::WAITING_FOR_PEER);

    // Add a small random delay to prevent collision if multiple badges
    // start simultaneously (stagger the transmissions)
    vTaskDelay(pdMS_TO_TICKS(esp_random() % 200));

    // Set timeout (5 seconds)
    _ir_timeout_timestamp = esp_timer_get_time() / 1000 + 5000;

    // Send SYNC_REQUEST with our unique ID
    nsec::runtime::badge_unique_id id = nsec::g::the_badge->get_unique_id();
    _send_ir_packet(message::ir_packet_type::SYNC_REQUEST, id);

    // Start receiving
    uint8_t buffer[64];
    _ir_interface.receive(buffer, sizeof(buffer));
}

void nc::network_handler::_send_ir_packet(
    message::ir_packet_type type,
    const nsec::runtime::badge_unique_id &id) noexcept
{
    message::ir_packet packet;
    packet.type = type;

    // Copy MAC address
    memcpy(packet.mac, id.data(), 6);

    // Calculate checksum (simple XOR of all bytes)
    packet.checksum = 0;
    uint8_t *data = reinterpret_cast<uint8_t *>(&packet);
    for (size_t i = 0; i < sizeof(message::ir_packet) - sizeof(uint16_t); i++) {
        packet.checksum ^= data[i];
    }

    try {
        _ir_interface.send(reinterpret_cast<uint8_t *>(&packet),
                           sizeof(packet));
        _logger.info("Sent IR packet type: {}", static_cast<int>(type));
    } catch (const nsec::exception::runtime_error &e) {
        _logger.error("Failed to send IR packet: {}", e.what());
    }
}

void nc::network_handler::_process_received_ir_packet(
    const message::ir_packet *packet) noexcept
{
    nsec::runtime::badge_unique_id sender_id;
    memcpy(sender_id.data(), packet->mac, 6);

    _logger.info("Received IR packet type: {} from {}",
                 static_cast<int>(packet->type), sender_id);

    // Only accept packets from the same peer during an active exchange
    if (_current_ir_protocol_state != ir_protocol_state::IDLE &&
        _current_ir_protocol_state != ir_protocol_state::WAITING_FOR_PEER &&
        memcmp(sender_id.data(), _peer_id.data(), 6) != 0) {

        _logger.warn("Ignoring packet from unexpected peer: {}", sender_id);
        return;
    }

    // Get our badge ID
    nsec::runtime::badge_unique_id my_id = nsec::g::the_badge->get_unique_id();
    bool update_display = false;

    // Handle CANCEL messages in any state
    if (packet->type == message::ir_packet_type::CANCEL) {
        _logger.info("Received CANCEL from peer");
        _set_ir_protocol_state(ir_protocol_state::IDLE);
        nsec::g::the_badge->handle_ir_timeout();
        return;
    }

    switch (_current_ir_protocol_state) {
    case ir_protocol_state::IDLE:
        if (packet->type == message::ir_packet_type::SYNC_REQUEST) {
            _logger.info("Received SYNC_REQUEST while IDLE, becoming RECEIVER");
            _peer_id = sender_id;
            _set_ir_protocol_state(ir_protocol_state::RECEIVER);
            update_display = true;

            _ir_timeout_timestamp = esp_timer_get_time() / 1000 + 5000;

            _send_ir_packet(message::ir_packet_type::SYNC_ACCEPT, my_id);
        }
        break;

    case ir_protocol_state::WAITING_FOR_PEER:
        if (packet->type == message::ir_packet_type::SYNC_REQUEST) {
            // Conflict resolution: compare IDs to determine roles
            bool i_am_lower = memcmp(my_id.data(), sender_id.data(), 6) < 0;

            _logger.info("SYNC_REQUEST conflict, I am {} than peer",
                         i_am_lower ? "lower" : "higher");

            if (i_am_lower) {
                // Lower ID becomes receiver
                _peer_id = sender_id;
                _set_ir_protocol_state(ir_protocol_state::RECEIVER);
                update_display = true;

                _ir_timeout_timestamp = esp_timer_get_time() / 1000 + 5000;

                _send_ir_packet(message::ir_packet_type::SYNC_ACCEPT, my_id);
            } else {
                // Higher ID stays in waiting state
                _logger.info("Remaining in WAITING state due to ID comparison");
            }
        } else if (packet->type == message::ir_packet_type::SYNC_ACCEPT) {
            _logger.info("Received SYNC_ACCEPT, becoming SENDER");
            _peer_id = sender_id;
            _set_ir_protocol_state(ir_protocol_state::SENDER);
            update_display = true;

            _ir_timeout_timestamp = esp_timer_get_time() / 1000 + 5000;

            _send_ir_packet(message::ir_packet_type::ID_DATA, my_id);
        }
        break;

    case ir_protocol_state::SENDER:
        if (packet->type == message::ir_packet_type::ACK) {
            _logger.info("Received ACK as SENDER, exchange complete");

            nsec::g::the_badge->apply_score_change(1);

            _set_ir_protocol_state(ir_protocol_state::COMPLETED);
            update_display = true;

            vTaskDelay(pdMS_TO_TICKS(3000));
            _set_ir_protocol_state(ir_protocol_state::IDLE);
            nsec::g::the_badge->update_display();
        } else if (packet->type == message::ir_packet_type::SYNC_ACCEPT) {
            _logger.info("Received duplicate SYNC_ACCEPT, resending ID_DATA");
            _send_ir_packet(message::ir_packet_type::ID_DATA, my_id);
        }
        break;

    case ir_protocol_state::RECEIVER:
        if (packet->type == message::ir_packet_type::ID_DATA) {
            _logger.info("Received ID_DATA as RECEIVER, sending ACK");

            _send_ir_packet(message::ir_packet_type::ACK, my_id);

            nsec::g::the_badge->apply_score_change(1);

            _set_ir_protocol_state(ir_protocol_state::COMPLETED);
            update_display = true;

            vTaskDelay(pdMS_TO_TICKS(3000));
            _set_ir_protocol_state(ir_protocol_state::IDLE);
            nsec::g::the_badge->update_display();
        } else if (packet->type == message::ir_packet_type::SYNC_REQUEST) {
            _logger.info(
                "Received duplicate SYNC_REQUEST, resending SYNC_ACCEPT");
            _send_ir_packet(message::ir_packet_type::SYNC_ACCEPT, my_id);
        }
        break;

    case ir_protocol_state::COMPLETED:
        _logger.debug("Ignoring packet in COMPLETED state");
        break;
    }

    if (update_display) {
        nsec::g::the_badge->update_ir_exchange_status(
            _current_ir_protocol_state);
    }
}

void nc::network_handler::_check_ir_timeouts() noexcept
{
    if (_current_ir_protocol_state != ir_protocol_state::IDLE &&
        _current_ir_protocol_state != ir_protocol_state::COMPLETED) {

        uint32_t current_time = esp_timer_get_time() / 1000;
        if (current_time > _ir_timeout_timestamp) {
            _logger.warn("IR exchange timed out after {} ms",
                         current_time - (_ir_timeout_timestamp - 5000));

            if (_current_ir_protocol_state == ir_protocol_state::SENDER ||
                _current_ir_protocol_state == ir_protocol_state::RECEIVER) {
                nsec::runtime::badge_unique_id my_id =
                    nsec::g::the_badge->get_unique_id();
                _send_ir_packet(message::ir_packet_type::CANCEL, my_id);
            }

            _set_ir_protocol_state(ir_protocol_state::IDLE);
            nsec::g::the_badge->handle_ir_timeout();
        }
    }
}

void nc::network_handler::_reset() noexcept
{
    _current_ir_protocol_state = ir_protocol_state::IDLE;
    _peer_id = nsec::runtime::badge_unique_id{};
    _ir_timeout_timestamp = 0;
}

void nc::network_handler::_setup() noexcept
{
    _logger.debug("Setting up network handler");

    const esp_timer_create_args_t timer_args = {
        .callback =
            [](void *arg) {
                static_cast<nc::network_handler *>(arg)->_check_ir_timeouts();
            },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "ir_timeout_timer",
        .skip_unhandled_events = false};

    esp_timer_handle_t timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle,
                                             500 * 1000)); // Check every 500ms
}
