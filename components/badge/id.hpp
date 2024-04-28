/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BADGE_ID_HPP
#define NSEC_BADGE_ID_HPP

#include <array>
#include <cstdint>

namespace nsec::runtime
{

using badge_unique_id = std::array<std::uint8_t, 6>;

} // namespace nsec::runtime

#endif // NSEC_BADGE_ID_HPP