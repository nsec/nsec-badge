/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_NETWORK_HANDLER_HPP
#define NSEC_NETWORK_HANDLER_HPP

namespace nsec::communication
{
using peer_id_t = std::uint8_t;

class network_handler
{
  public:
    enum class link_position : std::uint8_t {
        UNKNOWN = 0b00,
        LEFT_MOST = 0b01,
        RIGHT_MOST = 0b10,
        MIDDLE = 0b11
    };
    link_position position() const noexcept
    {
        return link_position(_current_position);
    };

  private:
    // Storage for a link_position enum
    link_position _current_position;
};
} // namespace nsec::communication

#endif // NSEC_NETWORK_HANDLER_HPP
