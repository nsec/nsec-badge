/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BADGE_ID_HPP
#define NSEC_BADGE_ID_HPP

#include <array>
#include <cstdint>

#include <fmt/format.h>

namespace nsec::runtime
{

using badge_unique_id = std::array<std::uint8_t, 6>;

} // namespace nsec::runtime

template <>
struct fmt::formatter<nsec::runtime::badge_unique_id>
    : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nsec::runtime::badge_unique_id badge_id, FormatContext &ctx)
    {
        return fmt::formatter<string_view>::format(
            fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                        badge_id[0], badge_id[1], badge_id[2], badge_id[3],
                        badge_id[4], badge_id[5]),
            ctx);
    }
};

#endif // NSEC_BADGE_ID_HPP