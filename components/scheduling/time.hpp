/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 *
 */

#ifndef NSEC_SCHEDULING_TIME_HPP
#define NSEC_SCHEDULING_TIME_HPP

#include <cstdint>

namespace nsec::scheduling
{

using absolute_time_ms = std::uint64_t;
using relative_time_ms = std::uint64_t;

} // namespace nsec::scheduling

#endif /* NSEC_SCHEDULING_TIME_HPP */