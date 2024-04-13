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

enum class peer_relative_position : std::uint8_t {
    LEFT,
    RIGHT,
};

using peer_id_t = std::uint8_t;

class network_handler : public scheduling::periodic_task<network_handler>
{
    friend class periodic_task<network_handler>;

  public:
    /*
     * The network handler implements a minimal network stack between badges.
     *
     * The network protocol (wire protocol) defines the concept of a wave
     * front direction: it is the direction in which the messages are currently
     * flowing along the chain of badges.
     *
     * When the network connections are established, the badges can broadcast
     * on a turn-by-turn basis. The wave front directions reverses when it
     * reaches the bounds of the chain of badges.
     *
     * When a badge broadcasts, it sends its message(s) and ends its
     * transmission by sending a "MONITOR" message. When a badge receives a
     * "MONITOR" message, it knows it can now safely broadcast to the next
     * badge: the badge to its right if it received "MONITOR" from the left, or
     * the badge to its left if it received "MONITOR" from the right.
     *
     * In order to detect transmission errors and truncated messages, all
     * messages are prefixed with a header that contains:
     *   - a 16-bit "magic" number,
     *   - an 8-bit message type identifier.
     *
     * Discovery
     * ---------
     *
     * The network protocol always starts in a "DISCOVERY" state.
     *
     * Given the state of the connections, a badge can know if it is:
     *   - the left-most badge (its connected to a badge on its right),
     *   - the right-most badge (its connected to a badge on its left),
     *   - in the middle (its connected on both sides).
     *
     * The pairing process involves three message types:
     *   - ANNOUNCE (contains the peer id of the badge announcing itself),
     *   - ANNOUNCE_REPLY (contains the count of peers in the network as
     *     determined by the right-most badge),
     *   - MONITOR (indicates the end of a transmission).
     *
     * The left-most badge has the responsability of starting the pairing
     * process. It sends an "ANNOUNCE" message to the right, declaring itself
     * as peer 0, followed by a "MONITOR" message.
     *
     * The next badge receives the "ANNOUNCE" message, seeing that its left
     * neighbor has `peer_id = 0`: it can deduce that it, itself, has
     * `peer_id = 1`. When it receives the "MONITOR" message, it knows it is
     * its turn to send its own "ANNOUNCE" message to the right (if it has a
     * neighbor to the right), followed by a "MONITOR" message.
     *
     * Supposing the next badge is the right-most badge in the chain, it
     * receives an "ANNOUNCE" message with `peer_id = 1`, assigns itself
     * `peer_id = 2`, and receives the "MONITOR" message.
     *
     * As it is the right-most badge, it will reverse the flow of messages from
     * left-to-right to right-to-left and send an "ANNOUNCE_REPLY" message. The
     * right-most badge, knowing it is the third peer, can deduce that there
     * are 3 badges in the network.
     *
     * The badges will forward the "ANNOUNCE_REPLY" message along the chain
     * until it reaches the left-most badge. As the badges receive
     * "ANNOUNCE_REPLY", they enter the protocol's "RUNNING" state.
     *
     * Running
     * -------
     *
     * While the network protocol is in the "RUNNING" state, badges propagate a
     * "MONITOR" message along the chain. When a badge received a "MONITOR"
     * message, it is its chance to broadcast.
     *
     * If a message was enqueued by the application layer, it is sent, followed
     * by a "MONITOR" message. The application layer has the responsability of
     * deciding if a message must be forwarded or swallowed.
     *
     * Reset
     * -----
     *
     * On every tick of the networking task, the badges "sense" the state of
     * their physical connections. If they detect a change (a new connection,
     * or a connection lost), they send a "RESET" message along the current
     * wave front without waiting for their turn.
     *
     * This allows a fast reset of the protocol, returning to the "DISCOVERY"
     * state. The networking task also uses a timeout to reset the protocol
     * state if it receives no activity for a long time.
     *
     * Retransmission
     * -----
     *
     * The protocol has built-in support for retransmissions since messages
     * can be corrupted during their transmission. Following the reception of
     * a properly formatted message, a badge replies with an "OK" message. This
     * allows the emitter to retransmit or "reset" the connection after a set
     * timeout period elapses.
     */
    enum class wire_protocol_state : std::uint8_t {
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
    using pairing_end_notifier = void (*)(peer_id_t, std::uint8_t);

    enum class application_message_action : std::uint8_t { OK, ERROR };
    // application_message_action (relative_position_of_peer, message_type,
    // message_payload)
    using message_received_notifier = application_message_action (*)(
        nsec::communication::message::type, const std::uint8_t *);
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

    std::uint8_t peer_count() const noexcept
    {
        return _peer_count;
    }

    enum class link_position : std::uint8_t {
        UNKNOWN = 0b00,
        LEFT_MOST = 0b01,
        RIGHT_MOST = 0b10,
        MIDDLE = 0b11
    };
    link_position position() const noexcept;

    enum class enqueue_message_result : std::uint8_t {
        QUEUED,
        UNCONNECTED,
        FULL
    };
    enqueue_message_result enqueue_app_message(peer_relative_position direction,
                                               std::uint8_t msg_type,
                                               const std::uint8_t *msg_payload);

  protected:
    void tick(scheduling::absolute_time_ms current_time_ms) noexcept;

  private:
    enum class message_reception_state : std::uint8_t {
        RECEIVE_MAGIC_BYTE_1,
        RECEIVE_MAGIC_BYTE_2,
        RECEIVE_HEADER,
        RECEIVE_PAYLOAD,
    };
    enum class message_transmission_state : std::uint8_t {
        NONE,
        ATTEMPT_SEND,
        // On timeout, retransmit until timeout
        WAIT_CONFIRMATION,
    };

    void _setup() noexcept;

    void _position(link_position new_role) noexcept;

    void _set_wire_protocol_state(wire_protocol_state state) noexcept;

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
    std::uint8_t _outgoing_message_size() const noexcept;
    void _clear_outgoing_message() noexcept;
    void _set_outgoing_message(
        nsec::scheduling::absolute_time_ms current_time_ms,
        std::uint8_t message_type,
        const std::uint8_t *message_payload = nullptr) noexcept;

    // Message transmission request from an application.
    peer_relative_position
    _pending_outgoing_app_message_direction() const noexcept;
    std::uint8_t _pending_outgoing_app_message_size() const noexcept;
    bool _has_pending_outgoing_app_message() const noexcept;
    void _clear_pending_outgoing_app_message() noexcept;

    enum class check_connections_result : std::uint8_t {
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

    enum class handle_reception_result : std::uint8_t {
        NO_DATA,
        INCOMPLETE,
        COMPLETE,
        CORRUPTED,
    };
    handle_reception_result
    _handle_reception(uart_interface &, std::uint8_t &message_type,
                      std::uint8_t *message_payload) noexcept;

    enum class handle_transmission_result : std::uint8_t {
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

    bool _is_left_connected;
    bool _is_right_connected;

    // Storage for a link_position enum
    link_position _current_position;
    // Storage for a wire_protocol_state enum
    wire_protocol_state _current_wire_protocol_state;
    // Storage for a peer_relative_location enum. Indicates the direction of the
    // wave front by the time we get the next message.
    peer_relative_position _current_wave_front_direction;
    // Number of ticks in the current wire protocol states (used by states that
    // need to wait).
    std::uint8_t _ticks_in_wire_state;

    peer_relative_position _current_listening_side;

    // This node's unique id in the network.
    peer_id_t _peer_id;
    // Number of peers in the network (including this node).
    std::uint8_t _peer_count;

    message_reception_state _current_message_reception_state;
    // Number of bytes left to receive for the current message
    std::uint8_t _payload_bytes_to_receive;

    message_transmission_state _current_message_transmission_state;
    std::uint8_t _current_message_being_sent_size;

    peer_relative_position _current_message_being_sent_direction;
    std::uint8_t _current_message_being_sent_type;

    // App-level enqueued message
    peer_relative_position _current_pending_outgoing_app_message_direction;
    std::uint8_t _current_pending_outgoing_app_message_size;
    // Enqueued outgoing application message type and payload.
    std::uint8_t _current_pending_outgoing_app_message_type;
    std::uint8_t _current_pending_outgoing_app_message_payload
        [nsec::config::communication::protocol_max_message_size];

    // Message currently being sent and potentially retransmitted.
    nsec::scheduling::absolute_time_ms _last_transmission_time_ms;
    std::uint8_t _current_message_being_sent
        [nsec::config::communication::protocol_max_message_size];
    nsec::logging::logger _logger;
};
} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
