// SPDX-FileCopyrightText: 2023 NorthSec
//
// SPDX-License-Identifier: MIT

#ifndef NSEC_GLOBALS_HPP
#define NSEC_GLOBALS_HPP

#include "badge.hpp"
#include "scheduling/task.hpp"
#include "utils/config.hpp"
#include <cstdint>

namespace nsec::g
{
extern runtime::badge *the_badge;
} // namespace nsec::g

#endif /* NSEC_GLOBALS_HPP */
