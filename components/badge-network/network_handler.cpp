/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "network_handler.hpp"
#include "scheduling/time.hpp"
#include "utils/config.hpp"
#include <cstdint>

#include <badge/globals.hpp>

#include <cstring>

namespace ns = nsec::scheduling;
namespace nc = nsec::communication;

namespace
{
enum class wire_msg_type : std::uint8_t {
    // Reserved.
    NONE = 0,
    // Wire protocol reserved messages
    MONITOR = 3,
    RESET = 4,
    ANNOUNCE = 5,
    ANNOUNCE_REPLY = 6,
    OK = 7,

    // Application messages (forwarded to the application layer)
    // ...
};
}

namespace
{

constexpr std::uint8_t wire_protocol_magic_1 = 0b10101111;
constexpr std::uint8_t wire_protocol_magic_2 = 0b11111010;

struct wire_msg_header {
    // Message start with a 16-bit magic number to re-sync on message frames.
    std::uint8_t type;

    /*
     * 16-bit fletcher checksum, see
     * https://en.wikipedia.org/wiki/Fletcher%27s_checksum#Fletcher-16
     *
     * Checksum includes: type and payload bytes
     */
    std::uint16_t checksum;
} __attribute__((packed));

struct wire_msg_announce {
    std::uint8_t peer_id;
} __attribute__((packed));

struct wire_msg_announce_reply {
    std::uint8_t peer_count;
} __attribute__((packed));

std::uint8_t wire_msg_payload_size(std::uint8_t type)
{
    switch (wire_msg_type(type)) {
    case wire_msg_type::ANNOUNCE:
        return sizeof(wire_msg_announce);
    case wire_msg_type::ANNOUNCE_REPLY:
        return sizeof(wire_msg_announce_reply);
    case wire_msg_type::MONITOR:
    case wire_msg_type::RESET:
    case wire_msg_type::OK:
        return 0;
    default:
        switch (nc::message::type(type)) {
        case nc::message::type::ANNOUNCE_BADGE_ID:
            return sizeof(nc::message::announce_badge_id);
        default:
            break;
        }

        return 0;
    }
}

class fletcher16_checksumer
{
  public:
    void push(std::uint8_t value)
    {
        _sum_low += value;
        _sum_high += _sum_low;
    }

    std::uint16_t checksum() const noexcept
    {
        return (std::uint16_t(_sum_high) << 8) | std::uint16_t(_sum_low);
    }

  private:
    std::uint8_t _sum_low = 0;
    std::uint8_t _sum_high = 0;
};

void send_wire_magic(nc::SoftwareSerial &serial) noexcept
{
    serial.write(wire_protocol_magic_1);
    serial.write(wire_protocol_magic_2);
}

void send_wire_header(nc::SoftwareSerial &serial, std::uint8_t msg_type,
                      std::uint16_t checksum) noexcept
{
    const wire_msg_header header = {.type = msg_type, .checksum = checksum};

    send_wire_magic(serial);
    serial.write(reinterpret_cast<const std::uint8_t *>(&header),
                 sizeof(header));
}

void send_wire_msg(nc::SoftwareSerial &serial, std::uint8_t msg_type,
                   const std::uint8_t *msg = nullptr,
                   std::uint8_t msg_payload_size = 0) noexcept
{
    fletcher16_checksumer checksummer;

    checksummer.push(msg_type);
    for (std::uint8_t i = 0; i < msg_payload_size; i++) {
        checksummer.push(msg[i]);
    }

    send_wire_header(serial, msg_type, checksummer.checksum());
    serial.write(msg, msg_payload_size);
}

void send_wire_reset_msg(nc::SoftwareSerial &serial) noexcept
{
    send_wire_msg(serial, std::uint8_t(wire_msg_type::RESET));
}

void send_wire_ok_msg(nc::SoftwareSerial &serial) noexcept
{
    send_wire_msg(serial, std::uint8_t(wire_msg_type::OK));
}

ns::absolute_time_ms get_current_absolute_time()
{
    return 0;
}
} /* namespace */

nc::network_handler::network_handler() noexcept
    : ns::periodic_task<network_handler>(
          nsec::config::communication::network_handler_base_period_ms),
      _left_serial(nsec::config::communication::serial_rx_pin_left,
                   nsec::config::communication::serial_tx_pin_left, true),
      _right_serial(nsec::config::communication::serial_rx_pin_right,
                    nsec::config::communication::serial_tx_pin_right, true),
      _is_left_connected{false}, _is_right_connected{false},
      _current_wire_protocol_state{
          std::uint8_t(wire_protocol_state::UNCONNECTED)}
{
    _reset();
    // FIXME Start task
    // ng::the_scheduler.schedule_task(*this);
}

void nc::network_handler::setup() noexcept
{
    // Init badge ports to sense connecting badges
    // TODO

    // Initialize serial devices
    _left_serial.begin(nsec::config::communication::software_serial_speed);
    _right_serial.begin(nsec::config::communication::software_serial_speed);
}

bool nc::network_handler::_sense_is_left_connected() const noexcept
{
    // We hope/assume that we'll see the other side's TX pin low when idle.
    return false;
}

bool nc::network_handler::_sense_is_right_connected() const noexcept
{
    return false;
}

nc::network_handler::link_position
nc::network_handler::position() const noexcept
{
    return link_position(_current_position);
}

void nc::network_handler::_reset() noexcept
{
    _position(link_position::UNKNOWN);
    _wire_protocol_state(wire_protocol_state::UNCONNECTED);
    _is_left_connected = false;
    _is_right_connected = false;
}

void nc::network_handler::_detect_and_set_position() noexcept
{
    const auto connection_mask =
        (_is_left_connected << 1) | _is_right_connected;
    const link_position new_position[] = {[0b00] = link_position::UNKNOWN,
                                          [0b01] = link_position::LEFT_MOST,
                                          [0b10] = link_position::RIGHT_MOST,
                                          [0b11] = link_position::MIDDLE};

    _position(new_position[connection_mask]);
}

nc::network_handler::check_connections_result
nc::network_handler::_check_connections() noexcept
{
    const bool left_is_connected = _sense_is_left_connected();
    const bool right_is_connected = _sense_is_right_connected();
    const bool left_was_connected = _is_left_connected;
    const bool right_was_connected = _is_right_connected;
    const bool left_state_changed = left_is_connected != left_was_connected;
    const bool right_state_changed = right_is_connected != right_was_connected;
    const bool topology_changed = left_state_changed || right_state_changed;

    if (!topology_changed) {
        return check_connections_result::NO_CHANGE;
    } else {
        nc::SoftwareSerial *destination_serial;
        switch (position()) {
        case link_position::LEFT_MOST:
            destination_serial = &_right_serial;
            break;
        case link_position::RIGHT_MOST:
            destination_serial = &_left_serial;
            break;
        default:
            destination_serial = nullptr;
        }

        if (_wire_protocol_state() != wire_protocol_state::UNCONNECTED &&
            destination_serial) {
            // Bypass the "send" state machine since this is just a hint to
            // speed up recovery.
            send_wire_reset_msg(*destination_serial);
        }
    }

    _reset();

    _is_left_connected = left_is_connected;
    _is_right_connected = right_is_connected;
    _detect_and_set_position();
    return check_connections_result::TOPOLOGY_CHANGED;
}

void nc::network_handler::_position(link_position new_position) noexcept
{
    _current_position = std::uint8_t(new_position);
    switch (position()) {
    case link_position::UNKNOWN:
        break;
    case link_position::LEFT_MOST:
        _listening_side(peer_relative_position::RIGHT);
        // We are peer 0 (i.e. the left-most node) and will
        // initiate the discovery.
        _wire_protocol_state(wire_protocol_state::WAIT_TO_INITIATE_DISCOVERY);
        _peer_id = 0;
        break;
    case link_position::RIGHT_MOST:
    case link_position::MIDDLE:
        // A peer on our left will initiate the discovery: wait for the message.
        _listening_side(peer_relative_position::LEFT);
        _wire_protocol_state(wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE);
        break;
    }
}

nc::peer_relative_position nc::network_handler::_listening_side() const noexcept
{
    return peer_relative_position(_current_listening_side);
}

void nc::network_handler::_listening_side(peer_relative_position side) noexcept
{
    _current_listening_side = std::uint8_t(side);
    switch (_listening_side()) {
    case peer_relative_position::LEFT:
        _left_serial.listen();
        break;
    case peer_relative_position::RIGHT:
        _right_serial.listen();
        break;
    }
}

void nc::network_handler::_reverse_listening_side() noexcept
{
    _listening_side(_listening_side() == peer_relative_position::LEFT
                        ? peer_relative_position::RIGHT
                        : peer_relative_position::LEFT);
}

void nc::network_handler::_reverse_wave_front_direction() noexcept
{
    _wave_front_direction(_wave_front_direction() ==
                                  peer_relative_position::LEFT
                              ? peer_relative_position::RIGHT
                              : peer_relative_position::LEFT);
}

nc::network_handler::wire_protocol_state
nc::network_handler::_wire_protocol_state() const noexcept
{
    return wire_protocol_state(_current_wire_protocol_state);
}

bool nc::network_handler::_is_wire_protocol_in_a_running_state(
    wire_protocol_state state) noexcept
{
    return state >= wire_protocol_state::RUNNING_RECEIVE_MESSAGE &&
           state <= wire_protocol_state::RUNNING_CONFIRM_MONITOR;
}

void nc::network_handler::_wire_protocol_state(
    wire_protocol_state state) noexcept
{
    const auto previous_protocol_state =
        wire_protocol_state(_current_wire_protocol_state);

    _current_wire_protocol_state = std::uint8_t(state);
    _ticks_in_wire_state = 0;
    // Reset timeout timestamp.
    _last_message_received_time_ms = get_current_absolute_time();

    if (_is_wire_protocol_in_a_running_state(previous_protocol_state) &&
        state == wire_protocol_state::UNCONNECTED) {
        nsec::g::the_badge.on_disconnection();
    }

    if (state == wire_protocol_state::UNCONNECTED) {
        _current_pending_outgoing_app_message_size = 0;
        // We are a sad and lonely node hacking together a network protocol.
        _peer_count = 1;
        // Unknown peer id.
        _peer_id = 0;
        _wave_front_direction(peer_relative_position::RIGHT);
        _message_reception_state(message_reception_state::RECEIVE_MAGIC_BYTE_1);
        _clear_outgoing_message();
        _clear_pending_outgoing_app_message();

        /* Empty the serial buffers by switching listening side. */
        _right_serial.listen();
        _left_serial.listen();
    }

    if (!_is_wire_protocol_in_a_running_state(previous_protocol_state) &&
        _is_wire_protocol_in_a_running_state(state)) {
        // Discovery has completed.
        nsec::g::the_badge.on_pairing_end(_peer_id, _peer_count);
    }
}

nc::peer_relative_position
nc::network_handler::_wave_front_direction() const noexcept
{
    return peer_relative_position(_current_wave_front_direction);
}

void nc::network_handler::_wave_front_direction(
    peer_relative_position new_direction) noexcept
{
    _current_wave_front_direction = std::uint8_t(new_direction);
}

nc::network_handler::message_reception_state
nc::network_handler::_message_reception_state() const noexcept
{
    return message_reception_state(_current_message_reception_state);
}

void nc::network_handler::_message_reception_state(
    message_reception_state new_state) noexcept
{
    _current_message_reception_state = std::uint8_t(new_state);
}

nc::network_handler::message_transmission_state
nc::network_handler::_message_transmission_state() const noexcept
{
    return message_transmission_state(_current_message_transmission_state);
}

void nc::network_handler::_message_transmission_state(
    message_transmission_state new_state) noexcept
{
    _current_message_transmission_state = std::uint8_t(new_state);
}

nc::peer_relative_position
nc::network_handler::_outgoing_message_direction() const noexcept
{
    return peer_relative_position(_current_message_being_sent_direction);
}

void nc::network_handler::_outgoing_message_direction(
    peer_relative_position direction) noexcept
{
    _current_message_being_sent_direction = std::uint8_t(direction);
}

std::uint8_t nc::network_handler::_outgoing_message_size() const noexcept
{
    return _current_message_being_sent_size;
}

void nc::network_handler::_clear_outgoing_message() noexcept
{
    _current_message_being_sent_size = 0;
    _message_transmission_state(message_transmission_state::NONE);
}

void nc::network_handler::_set_outgoing_message(
    nsec::scheduling::absolute_time_ms current_time_ms,
    std::uint8_t message_type, const std::uint8_t *message_payload) noexcept
{
    _current_message_being_sent_size = wire_msg_payload_size(message_type);
    memcpy(_current_message_being_sent, message_payload,
           _current_message_being_sent_size);
    switch (position()) {
    case link_position::LEFT_MOST:
        _outgoing_message_direction(peer_relative_position::RIGHT);
        break;
    case link_position::RIGHT_MOST:
        _outgoing_message_direction(peer_relative_position::LEFT);
        break;
    case link_position::MIDDLE:
        _outgoing_message_direction(_wave_front_direction());
        break;
    default:
        // Unreachable.
        return;
    }

    _current_message_being_sent_type = message_type;
    _message_transmission_state(message_transmission_state::ATTEMPT_SEND);
    _last_transmission_time_ms = current_time_ms;
}

nc::peer_relative_position
nc::network_handler::_pending_outgoing_app_message_direction() const noexcept
{
    return peer_relative_position(
        _current_pending_outgoing_app_message_direction);
}

std::uint8_t
nc::network_handler::_pending_outgoing_app_message_size() const noexcept
{
    return _current_pending_outgoing_app_message_size;
}

bool nc::network_handler::_has_pending_outgoing_app_message() const noexcept
{
    return _current_pending_outgoing_app_message_type != 0;
}

void nc::network_handler::_clear_pending_outgoing_app_message() noexcept
{
    _current_pending_outgoing_app_message_type = 0;
}

nc::SoftwareSerial &nc::network_handler::_listening_side_serial() noexcept
{
    return _listening_side() == peer_relative_position::LEFT ? _left_serial
                                                             : _right_serial;
}

nc::network_handler::handle_reception_result
nc::network_handler::_handle_reception(nc::SoftwareSerial &serial,
                                       std::uint8_t &message_type,
                                       std::uint8_t *message_payload) noexcept
{
    bool saw_data = false;

    while (serial.available()) {
        saw_data = true;

        switch (_message_reception_state()) {
        case message_reception_state::RECEIVE_MAGIC_BYTE_1: {
            const auto front_byte = std::uint8_t(serial.read());
            if (front_byte != wire_protocol_magic_1) {
                break;
            }

            _message_reception_state(
                message_reception_state::RECEIVE_MAGIC_BYTE_2);
            break;
        }
        case message_reception_state::RECEIVE_MAGIC_BYTE_2: {
            const auto front_byte = std::uint8_t(serial.read());

            if (front_byte != wire_protocol_magic_2) {
                break;
            }

            _message_reception_state(message_reception_state::RECEIVE_HEADER);
            break;
        }
        case message_reception_state::RECEIVE_HEADER: {
            if (serial.available() < int(sizeof(wire_msg_header))) {
                return handle_reception_result::INCOMPLETE;
            }

            const wire_msg_header header = {.type =
                                                std::uint8_t(serial.peek())};

            const auto msg_type = header.type;

            const auto msg_payload_size = wire_msg_payload_size(msg_type);
            _message_reception_state(message_reception_state::RECEIVE_PAYLOAD);
            /*
             * Keep the payload and the header's type byte which will allow
             * us to dispatch the message, and the checksym, which will allow us
             * to validate the message.
             */
            _payload_bytes_to_receive = msg_payload_size + sizeof(header);
            break;
        }
        case message_reception_state::RECEIVE_PAYLOAD: {
            if (serial.available() < _payload_bytes_to_receive) {
                return handle_reception_result::INCOMPLETE;
            }

            // Get ready to receive the beginning of the next message.
            _message_reception_state(
                message_reception_state::RECEIVE_MAGIC_BYTE_1);

            message_type = std::uint8_t(serial.read());
            const std::uint16_t checksum = std::uint16_t(serial.read()) |
                                           std::uint16_t(serial.read()) << 8;
            const auto payload_size = wire_msg_payload_size(message_type);

            if (payload_size != 0 && !message_payload) {
                // Caller doesn't expect a payload.
                return handle_reception_result::CORRUPTED;
            }

            if (payload_size != 0) {
                serial.readBytes(message_payload, payload_size);
            }

            // Validate checksum.
            fletcher16_checksumer checksummer;
            checksummer.push(message_type);
            for (std::uint8_t i = 0; i < payload_size; i++) {
                checksummer.push(message_payload[i]);
            }

            return checksum == checksummer.checksum()
                       ? handle_reception_result::COMPLETE
                       : handle_reception_result::CORRUPTED;
        }
        }
    }

    return saw_data ? handle_reception_result::INCOMPLETE
                    : handle_reception_result::NO_DATA;
}

nc::network_handler::handle_transmission_result
nc::network_handler::_handle_transmission(
    nsec::scheduling::absolute_time_ms current_time_ms) noexcept
{
    switch (_message_transmission_state()) {
    case message_transmission_state::NONE:
        return handle_transmission_result::COMPLETE;
    case message_transmission_state::ATTEMPT_SEND: {
        auto &sending_serial =
            _outgoing_message_direction() == peer_relative_position::LEFT
                ? _left_serial
                : _right_serial;

        // Listen before send since the other side can reply OK immediately.
        _listening_side(_outgoing_message_direction());
        send_wire_msg(sending_serial, _current_message_being_sent_type,
                      _current_message_being_sent,
                      _current_message_being_sent_size);
        _last_transmission_time_ms = current_time_ms;
        _message_transmission_state(
            message_transmission_state::WAIT_CONFIRMATION);
        break;
    }
    case message_transmission_state::WAIT_CONFIRMATION: {
        std::uint8_t new_message_type;
        auto &listening_serial =
            _outgoing_message_direction() == peer_relative_position::LEFT
                ? _left_serial
                : _right_serial;

        const auto receive_result =
            _handle_reception(listening_serial, new_message_type, nullptr);

        switch (receive_result) {
        case handle_reception_result::COMPLETE: {
            _clear_outgoing_message();
            return handle_transmission_result::COMPLETE;
        }
        case handle_reception_result::NO_DATA:
            if (current_time_ms - _last_transmission_time_ms >=
                nsec::config::communication::
                    network_handler_retransmit_timeout_ms) {

                // Attempt a retransmission.
                _message_transmission_state(
                    message_transmission_state::ATTEMPT_SEND);
            }

            break;
        default:
            // Assume the message was "OK".
            _clear_outgoing_message();
            return handle_transmission_result::COMPLETE;
            break;
        }

        break;
    }
    }

    return handle_transmission_result::INCOMPLETE;
}

nc::network_handler::enqueue_message_result
nc::network_handler::enqueue_app_message(peer_relative_position direction,
                                         std::uint8_t msg_type,
                                         const std::uint8_t *msg_payload)
{
    const auto payload_size = wire_msg_payload_size(msg_type);

    if (_has_pending_outgoing_app_message() ||
        payload_size > sizeof(_current_pending_outgoing_app_message_payload)) {
        return enqueue_message_result::FULL;
    }

    if (position() == link_position::LEFT_MOST &&
        direction == peer_relative_position::LEFT) {
        _reset();
        return enqueue_message_result::UNCONNECTED;
    } else if (position() == link_position::RIGHT_MOST &&
               direction == peer_relative_position::RIGHT) {
        _reset();
        return enqueue_message_result::UNCONNECTED;
    }

    _current_pending_outgoing_app_message_direction = std::uint8_t(direction);
    _current_pending_outgoing_app_message_size = payload_size;
    _current_pending_outgoing_app_message_type = msg_type;
    memcpy(_current_pending_outgoing_app_message_payload, msg_payload,
           payload_size);
    return enqueue_message_result::QUEUED;
}

bool nc::network_handler::_is_wire_protocol_in_a_reception_state(
    wire_protocol_state state) noexcept
{
    return state == wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE ||
           state ==
               wire_protocol_state::DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE ||
           state == wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE_REPLY ||
           state == wire_protocol_state::
                        DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE_REPLY ||
           state == wire_protocol_state::RUNNING_RECEIVE_MESSAGE;
}

void nc::network_handler::_run_wire_protocol(
    ns::absolute_time_ms current_time_ms) noexcept
{
    if (_last_message_received_time_ms < current_time_ms &&
        (current_time_ms - _last_message_received_time_ms) >
            nsec::config::communication::network_handler_timeout_ms &&
        _wire_protocol_state() != wire_protocol_state ::UNCONNECTED) {
        // No activity for a while... reset.
        _reset();
        return;
    }

    std::uint8_t message_type =
        4; // FIXME: Set to reset as a temporary build fix.
    std::uint8_t
        message_payload[nsec::config::communication::protocol_max_message_size -
                        sizeof(wire_msg_header)];

    if (_is_wire_protocol_in_a_reception_state(_wire_protocol_state())) {
        const auto receive_result = _handle_reception(
            _listening_side_serial(), message_type, message_payload);

        if (receive_result != handle_reception_result::COMPLETE) {
            /*
             * If the message is incomplete, we wait for the remaining data. If
             * the message is corrupted, we wait for a retransmission.
             */
            return;
        }

        _last_message_received_time_ms = current_time_ms;
        send_wire_ok_msg(_listening_side_serial());

        if (wire_msg_type(message_type) == wire_msg_type::RESET) {
            _reset();
            return;
        }
    }

    if (_message_transmission_state() != message_transmission_state::NONE) {
        const auto transmit_result = _handle_transmission(current_time_ms);
        if (transmit_result != handle_transmission_result::COMPLETE) {
            return;
        }
    }

    switch (_wire_protocol_state()) {
    case wire_protocol_state::UNCONNECTED:
        // Nothing to do.
        break;
    case wire_protocol_state::WAIT_TO_INITIATE_DISCOVERY:
        /*
         * State only reached by the left-most node.
         * Wait for the other boards to setup and expect our messages.
         */
        if (_ticks_in_wire_state++ == 3) {
            _wire_protocol_state(wire_protocol_state::DISCOVERY_SEND_ANNOUNCE);
        }

        break;
    case wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE: {
        if (wire_msg_type(message_type) != wire_msg_type::ANNOUNCE) {
            // Unexpected message: protocol error.
            _reset();
            return;
        }

        /*
         * Our left neighbor announced themselves which allows us to allocate
         * ourselves a peer id.
         */
        const auto *announce_msg =
            reinterpret_cast<const wire_msg_announce *>(message_payload);

        _peer_id = announce_msg->peer_id + 1;

        /*
         * Only valid for the right-most node, otherwise it will be overwriten
         * when ANNOUNCE_REPLY is received.
         */
        _peer_count = _peer_id + 1;
        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE);
        break;
    }
    case wire_protocol_state::DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE:
        if (wire_msg_type(message_type) != wire_msg_type::MONITOR) {
            // Unexpected message: protocol error.
            _reset();
            return;
        }

        // It is our turn to transmit.
        if (position() == link_position::MIDDLE) {
            // Announce ourselves to the right-side neighbor.
            _wire_protocol_state(wire_protocol_state::DISCOVERY_SEND_ANNOUNCE);
        } else {
            // We are the right-most node, initiate the announce reply.
            _wire_protocol_state(
                wire_protocol_state::DISCOVERY_SEND_ANNOUNCE_REPLY);
        }

        break;
    case wire_protocol_state::DISCOVERY_SEND_ANNOUNCE: {
        // Not reachable by the right-most node.
        const wire_msg_announce our_annouce_msg = {.peer_id = _peer_id};

        _set_outgoing_message(
            current_time_ms, std::uint8_t(wire_msg_type::ANNOUNCE),
            reinterpret_cast<const std::uint8_t *>(&our_annouce_msg));
        _wire_protocol_state(wire_protocol_state::DISCOVERY_CONFIRM_ANNOUNCE);
        break;
    }
    case wire_protocol_state::DISCOVERY_CONFIRM_ANNOUNCE:
        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE);
        break;
    case wire_protocol_state::DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE:
        // Not reachable by the right-most node.
        _set_outgoing_message(current_time_ms,
                              std::uint8_t(wire_msg_type::MONITOR));
        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE);
        break;
    case wire_protocol_state::DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE:
        _wave_front_direction(peer_relative_position::LEFT);

        // Next message (ANNOUNCE_REPLY) will come from our right neighbor.
        _listening_side(peer_relative_position::RIGHT);
        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE_REPLY);
        break;
    case wire_protocol_state::DISCOVERY_RECEIVE_ANNOUNCE_REPLY: {
        // Not reachable by the right-most node.
        const auto *announce_reply_msg =
            reinterpret_cast<const wire_msg_announce_reply *>(message_payload);

        _peer_count = announce_reply_msg->peer_count;
        _wire_protocol_state(
            wire_protocol_state::
                DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE_REPLY);
        break;
    }
    case wire_protocol_state::DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE_REPLY:
        if (wire_msg_type(message_type) != wire_msg_type::MONITOR) {
            // Unexpected message: protocol error.
            _reset();
            return;
        }

        if (position() == link_position::MIDDLE) {
            _wire_protocol_state(
                wire_protocol_state::DISCOVERY_SEND_ANNOUNCE_REPLY);
        } else {
            // We are the left-most node.
            _wave_front_direction(peer_relative_position::RIGHT);
            _wire_protocol_state(wire_protocol_state::RUNNING_SEND_APP_MESSAGE);
        }

        break;
    case wire_protocol_state::DISCOVERY_SEND_ANNOUNCE_REPLY: {
        const wire_msg_announce_reply announce_reply_msg = {.peer_count =
                                                                _peer_count};

        _set_outgoing_message(
            current_time_ms, std::uint8_t(wire_msg_type::ANNOUNCE_REPLY),
            reinterpret_cast<const std::uint8_t *>(&announce_reply_msg));

        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_CONFIRM_ANNOUNCE_REPLY);
        break;
    }
    case wire_protocol_state::DISCOVERY_CONFIRM_ANNOUNCE_REPLY:
        _wire_protocol_state(
            wire_protocol_state::DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE_REPLY);
        break;
    case wire_protocol_state::DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE_REPLY:
        _set_outgoing_message(current_time_ms,
                              std::uint8_t(wire_msg_type::MONITOR));
        _wire_protocol_state(
            wire_protocol_state::
                DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE_REPLY);
        break;
    case wire_protocol_state::DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE_REPLY:
        // Next message will come from the left.
        _listening_side(peer_relative_position::LEFT);
        _wave_front_direction(peer_relative_position::RIGHT);
        _wire_protocol_state(wire_protocol_state::RUNNING_RECEIVE_MESSAGE);
        break;
    case wire_protocol_state::RUNNING_RECEIVE_MESSAGE: {
        if (message_type >=
            nsec::config::communication::application_message_type_range_begin) {
            // Process app-level message
            nsec::g::the_badge.on_message_received(
                nc::message::type(message_type), message_payload);
        } else if (wire_msg_type(message_type) == wire_msg_type::MONITOR) {
            _wire_protocol_state(wire_protocol_state::RUNNING_SEND_APP_MESSAGE);
        } else {
            // Unexpected message or a reset message.
            _reset();
            return;
        }

        break;
    }
    case wire_protocol_state::RUNNING_SEND_APP_MESSAGE: {
        const auto is_middle_peer =
            position() == nc::network_handler::link_position::MIDDLE;
        const auto pending_matches_wave_front_direction =
            _pending_outgoing_app_message_direction() ==
            _wave_front_direction();

        if (_has_pending_outgoing_app_message() &&
            (!is_middle_peer ||
             (is_middle_peer && pending_matches_wave_front_direction))) {
            _set_outgoing_message(
                current_time_ms, _current_pending_outgoing_app_message_type,
                _current_pending_outgoing_app_message_payload);
            _clear_pending_outgoing_app_message();
            _wire_protocol_state(
                wire_protocol_state::RUNNING_CONFIRM_APP_MESSAGE);
        } else {
            // Nothing sent, skip the confirmation step.
            _wire_protocol_state(wire_protocol_state::RUNNING_SEND_MONITOR);
        }

        break;
    }
    case wire_protocol_state::RUNNING_CONFIRM_APP_MESSAGE:
        nsec::g::the_badge.on_app_message_sent();
        _wire_protocol_state(wire_protocol_state::RUNNING_SEND_MONITOR);
        break;
    case wire_protocol_state::RUNNING_SEND_MONITOR:
        _set_outgoing_message(current_time_ms,
                              std::uint8_t(wire_msg_type::MONITOR));
        _wire_protocol_state(wire_protocol_state::RUNNING_CONFIRM_MONITOR);
        break;
    case wire_protocol_state::RUNNING_CONFIRM_MONITOR:
        if (position() == link_position::MIDDLE) {
            _reverse_wave_front_direction();
        }

        _wire_protocol_state(wire_protocol_state::RUNNING_RECEIVE_MESSAGE);
        break;
    }
}

void nc::network_handler::tick(ns::absolute_time_ms current_time_ms) noexcept
{
    if (_check_connections() == check_connections_result::TOPOLOGY_CHANGED) {
        /*
         * The protocol state has been reset. Resume on the next tick
         * to allow our peers enough time to detect the change.
         */
        return;
    }

    if (_wire_protocol_state() == wire_protocol_state::UNCONNECTED) {
        return;
    }

    _run_wire_protocol(current_time_ms);
}
