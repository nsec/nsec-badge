/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_NETWORK_HANDLER_HPP
#define NSEC_NETWORK_HANDLER_HPP

#include "ir_interface.hpp"
#include "network_messages.hpp"
#include <badge/id.hpp>
#include <cstdint>

namespace nsec::communication
{

class network_handler
{
  public:
    enum class ir_protocol_state : uint8_t {
        IDLE,
        WAITING_FOR_PEER,
        SENDER,
        RECEIVER,
        COMPLETED,
        TIMEOUT,
    };

    network_handler() noexcept;
    void start_ir_key_exchange() noexcept;

    ir_protocol_state get_ir_protocol_state() const noexcept
    {
        return _current_ir_protocol_state;
    }
    nsec::runtime::badge_unique_id peer_id() const noexcept
    {
        return _peer_id;
    }

  private:
    ir_interface _ir_interface;
    ir_protocol_state _current_ir_protocol_state;
    nsec::runtime::badge_unique_id _peer_id;
    nsec::logging::logger _logger;
    uint32_t _ir_timeout_timestamp;

    void _reset() noexcept;
    void _setup() noexcept;
    void _set_ir_protocol_state(ir_protocol_state next_state) noexcept;
    void _send_ir_packet(message::ir_packet_type type,
                         const nsec::runtime::badge_unique_id &id) noexcept;
    void _process_received_ir_packet(const message::ir_packet *packet) noexcept;
    void _check_ir_timeouts() noexcept;
};

} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
