// SPDX-FileCopyrightText: 2023 NorthSec
//
// SPDX-License-Identifier: MIT

#include "globals.hpp"

nsec::scheduling::scheduler<nsec::config::scheduler::max_scheduled_task_count>
    nsec::g::the_scheduler;
nsec::runtime::badge nsec::g::the_badge;
