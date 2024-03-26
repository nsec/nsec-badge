/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge/globals.hpp>
#include <badge-button/watcher.hpp>
#include <utils/board.hpp>
#include <utils/config.hpp>

namespace nb = nsec::button;
namespace ns = nsec::scheduling;
namespace ng = nsec::g;
namespace nbb = nsec::board::button;

namespace
{
int digitalRead(int pin_id)
{
    return 0;
}
} // namespace

nb::watcher::watcher(nb::new_button_event_notifier new_button_notifier) noexcept
    : ns::periodic_task<watcher>(nsec::config::button::polling_period_ms),
      _notify_new_event{new_button_notifier}
{
    start();
}

void nb::watcher::setup() noexcept
{
    // Setup button pins
}

void nb::watcher::tick(
    [[maybe_unused]] ns::absolute_time_ms current_time_ms) noexcept
{
    const unsigned int btn_pins[] = {nbb::up,   nbb::right, nbb::down,
                                     nbb::left, nbb::ok,    nbb::cancel};
    constexpr auto btn_count = sizeof(btn_pins) / sizeof(*btn_pins);

    // Check the state of all button pins and debounce as needed
    for (auto btn_idx = 0U; btn_idx < btn_count; btn_idx++) {
        const auto new_event = digitalRead((int)btn_idx);

        _notify_new_event(static_cast<id>(btn_idx),
                          static_cast<event>(new_event));
    }
}
