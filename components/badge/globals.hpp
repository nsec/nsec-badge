// SPDX-FileCopyrightText: 2023 NorthSec
//
// SPDX-License-Identifier: MIT

#ifndef NSEC_GLOBALS_HPP
#define NSEC_GLOBALS_HPP

#include "badge.hpp"
#include "scheduling/scheduler.hpp"
#include "utils/config.hpp"
#include <cstdint>

namespace nsec::g
{
extern scheduling::scheduler<config::scheduler::max_scheduled_task_count>
    the_scheduler;
extern runtime::badge the_badge;
} // namespace nsec::g

#endif /* NSEC_GLOBALS_HPP */
