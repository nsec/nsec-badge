/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * Copyright 2025 Abdelhakim Qbaich <abdelhakim@qbaich.com>
 */

#ifndef NSEC_NETWORK_MESSAGES_HPP
#define NSEC_NETWORK_MESSAGES_HPP

#include "utils/config.hpp"
#include <cstdint>

namespace nsec::communication::message
{

enum class ir_packet_type : uint8_t {
    SYNC_REQUEST =
        nsec::config::communication::application_message_type_range_begin,
    SYNC_ACCEPT,
    ID_DATA,
    ACK,
    CANCEL,
};

struct ir_packet {
    ir_packet_type type;
    uint8_t mac[3];
    uint8_t checksum;
} __attribute__((packed));

} // namespace nsec::communication::message

#endif // NSEC_NETWORK_MESSAGES_HPP
