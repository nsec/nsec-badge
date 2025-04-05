/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_NETWORK_HANDLER_HPP
#define NSEC_NETWORK_HANDLER_HPP

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
        COMPLETED
    };

    ir_protocol_state get_ir_protocol_state() const noexcept
    {
        return _current_ir_protocol_state;
    }
    nsec::runtime::badge_unique_id peer_id() const noexcept
    {
        return _peer_id;
    }

  private:
    ir_protocol_state _current_ir_protocol_state = ir_protocol_state::IDLE;
    nsec::runtime::badge_unique_id _peer_id;
};

} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
