/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_NETWORK_HANDLER_HPP
#define NSEC_NETWORK_HANDLER_HPP

#include "network_messages.hpp"
#include "scheduling/task.hpp"
#include "uart_interface.hpp"
#include "utils/callback.hpp"
#include "utils/logging.hpp"

namespace nsec::communication
{

enum class peer_relative_position : uint8_t {
    LEFT,
    RIGHT,
};

using peer_id_t = uint8_t;

class network_handler : public scheduling::periodic_task<network_handler>
{
    friend class periodic_task<network_handler>;

  public:
    enum class wire_protocol_state : uint8_t {
        UNCONNECTED,
        /* Wait for boards to listen before left-most node initiates the
           discovery. */
        WAIT_TO_INITIATE_DISCOVERY,
        /* Discover left neighbours. */
        DISCOVERY_RECEIVE_ANNOUNCE,
        DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE,
        DISCOVERY_SEND_ANNOUNCE,
        DISCOVERY_CONFIRM_ANNOUNCE,
        DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE,
        DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE,
        DISCOVERY_RECEIVE_ANNOUNCE_REPLY,
        DISCOVERY_RECEIVE_MONITOR_AFTER_ANNOUNCE_REPLY,
        DISCOVERY_SEND_ANNOUNCE_REPLY,
        DISCOVERY_CONFIRM_ANNOUNCE_REPLY,
        DISCOVERY_SEND_MONITOR_AFTER_ANNOUNCE_REPLY,
        DISCOVERY_CONFIRM_MONITOR_AFTER_ANNOUNCE_REPLY,
        /* Waiting for application and protocol (MONITOR and RESET) messages. */
        RUNNING_RECEIVE_MESSAGE,
        RUNNING_SEND_APP_MESSAGE,
        RUNNING_CONFIRM_APP_MESSAGE,
        RUNNING_SEND_MONITOR,
        RUNNING_CONFIRM_MONITOR
    };

    using disconnection_notifier = void (*)();
    using pairing_begin_notifier = void (*)();
    // void (our_peer_id, peer count)
    using pairing_end_notifier = void (*)(peer_id_t, uint8_t);

    enum class application_message_action : uint8_t { OK, ERROR };
    // application_message_action (relative_position_of_peer, message_type,
    // message_payload)
    using message_received_notifier = application_message_action (*)(
        nsec::communication::message::type, const uint8_t *);
    using message_sent_notifier = void (*)();

    network_handler() noexcept;

    /* Deactivate copy and assignment. */
    network_handler(const network_handler &) = delete;
    network_handler(network_handler &&) = delete;
    network_handler &operator=(const network_handler &) = delete;
    network_handler &operator=(network_handler &&) = delete;
    ~network_handler() = default;

    peer_id_t peer_id() const noexcept
    {
        return _peer_id;
    }

    uint8_t peer_count() const noexcept
    {
        return _peer_count;
    }

    enum class link_position : uint8_t {
        UNKNOWN = 0b00,
        LEFT_MOST = 0b01,
        RIGHT_MOST = 0b10,
        MIDDLE = 0b11
    };
    link_position position() const noexcept;

    enum class enqueue_message_result : uint8_t { QUEUED, UNCONNECTED, FULL };
    enqueue_message_result enqueue_app_message(peer_relative_position direction,
                                               uint8_t msg_type,
                                               const uint8_t *msg_payload);

  protected:
    void tick(scheduling::absolute_time_ms current_time_ms) noexcept;

  private:

    enum class message_reception_state : uint8_t {
        RECEIVE_MAGIC_BYTE_1,
        RECEIVE_MAGIC_BYTE_2,
        RECEIVE_HEADER,
        RECEIVE_PAYLOAD,
    };
    enum class message_transmission_state : uint8_t {
        NONE,
        ATTEMPT_SEND,
        // On timeout, retransmit until timeout
        WAIT_CONFIRMATION,
    };

    void _setup() noexcept;

    void _position(link_position new_role) noexcept;

    wire_protocol_state _wire_protocol_state() const noexcept;
    void _wire_protocol_state(wire_protocol_state state) noexcept;

    peer_relative_position _wave_front_direction() const noexcept;
    void _wave_front_direction(peer_relative_position) noexcept;
    void _reverse_wave_front_direction() noexcept;

    peer_relative_position _listening_side() const noexcept;
    uart_interface &_listening_side_serial() noexcept;
    void _listening_side(peer_relative_position side) noexcept;
    void _reverse_listening_side() noexcept;

    message_reception_state _message_reception_state() const noexcept;
    void _message_reception_state(message_reception_state new_state) noexcept;

    // Message tranmsmission state machine.
    message_transmission_state _message_transmission_state() const noexcept;
    void
    _message_transmission_state(message_transmission_state new_state) noexcept;
    peer_relative_position _outgoing_message_direction() const noexcept;
    void _outgoing_message_direction(peer_relative_position) noexcept;
    uint8_t _outgoing_message_size() const noexcept;
    void _clear_outgoing_message() noexcept;
    void
    _set_outgoing_message(nsec::scheduling::absolute_time_ms current_time_ms,
                          uint8_t message_type,
                          const uint8_t *message_payload = nullptr) noexcept;

    // Message transmission request from an application.
    peer_relative_position
    _pending_outgoing_app_message_direction() const noexcept;
    uint8_t _pending_outgoing_app_message_size() const noexcept;
    bool _has_pending_outgoing_app_message() const noexcept;
    void _clear_pending_outgoing_app_message() noexcept;

    enum class check_connections_result : uint8_t {
        NO_CHANGE,
        TOPOLOGY_CHANGED,
    };
    check_connections_result _check_connections() noexcept;

    void _detect_and_set_position() noexcept;
    void _run_wire_protocol(
        nsec::scheduling::absolute_time_ms current_time_ms) noexcept;
    void _reset() noexcept;

    bool _sense_is_left_connected() const noexcept;
    bool _sense_is_right_connected() const noexcept;

    enum class handle_reception_result : uint8_t {
        NO_DATA,
        INCOMPLETE,
        COMPLETE,
        CORRUPTED,
    };
    handle_reception_result
    _handle_reception(uart_interface &, uint8_t &message_type,
                      uint8_t *message_payload) noexcept;

    enum class handle_transmission_result : uint8_t {
        COMPLETE,
        INCOMPLETE,
    };
    handle_transmission_result _handle_transmission(
        nsec::scheduling::absolute_time_ms current_time_ms) noexcept;

    static bool
    _is_wire_protocol_in_a_reception_state(wire_protocol_state state) noexcept;
    static bool
    _is_wire_protocol_in_a_running_state(wire_protocol_state state) noexcept;
    static void _log_wire_protocol_state(wire_protocol_state state) noexcept;
    static void
    _log_message_reception_state(message_reception_state state) noexcept;
    static void
    _log_message_transmission_state(message_transmission_state state) noexcept;

    uart_interface _left_serial;
    uart_interface _right_serial;
    nsec::scheduling::absolute_time_ms _last_message_received_time_ms;

    uint8_t _is_left_connected : 1;
    uint8_t _is_right_connected : 1;

    // Storage for a link_position enum
    uint8_t _current_position : 2;
    // Storage for a wire_protocol_state enum
    uint8_t _current_wire_protocol_state : 5;
    // Storage for a peer_relative_location enum. Indicates the direction of the
    // wave front by the time we get the next message.
    uint8_t _current_wave_front_direction : 1;
    // Number of ticks in the current wire protocol states (used by states that
    // need to wait).
    uint8_t _ticks_in_wire_state : 2;
    // Storage for a peer_relative_location enum
    uint8_t _current_listening_side : 1;

    // This node's unique id in the network.
    peer_id_t _peer_id : 5;
    // Number of peers in the network (including this node).
    uint8_t _peer_count : 5;

    // Storage for a message_reception_state enum
    uint8_t _current_message_reception_state : 3;
    // Number of bytes left to receive for the current message
    uint8_t _payload_bytes_to_receive : 5;

    // Storage for a message_transmission_state enum
    uint8_t _current_message_transmission_state : 2;
    uint8_t _current_message_being_sent_size : 5;
    // Storage for a peer_relative_location enum
    uint8_t _current_message_being_sent_direction : 1;
    uint8_t _current_message_being_sent_type : 5;

    // App-level enqueued message
    // Storage for a peer_relative_location enum
    uint8_t _current_pending_outgoing_app_message_direction : 1;
    uint8_t _current_pending_outgoing_app_message_size : 5;
    // Enqueued outgoing application message type and payload.
    uint8_t _current_pending_outgoing_app_message_type : 5;
    uint8_t _current_pending_outgoing_app_message_payload
        [nsec::config::communication::protocol_max_message_size];

    // Message currently being sent and potentially retransmitted.
    nsec::scheduling::absolute_time_ms _last_transmission_time_ms;
    uint8_t _current_message_being_sent
        [nsec::config::communication::protocol_max_message_size];
    nsec::logging::logger _logger;
};
} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
