/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_NETWORK_MESSAGES_HPP
#define NSEC_NETWORK_MESSAGES_HPP

#include "utils/config.hpp"
#include <cstdint>

namespace nsec::communication::message
{

enum class type : uint8_t {
    ANNOUNCE_BADGE_ID =
        nsec::config::communication::application_message_type_range_begin,
    PAIRING_ANIMATION_PART_1_DONE,
    PAIRING_ANIMATION_PART_2_DONE,
    PAIRING_ANIMATION_DONE,
};

struct announce_badge_id {
    std::uint8_t peer_id;
    std::uint8_t board_unique_id[10];
} __attribute__((packed));

} // namespace nsec::communication::message

#endif // NSEC_NETWORK_MESSAGES_HPP
