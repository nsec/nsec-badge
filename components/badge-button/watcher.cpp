/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge/globals.hpp>
#include <badge-button/watcher.hpp>
#include <utils/board.hpp>
#include <utils/config.hpp>
#include "buttons.h"

static badge_button_event_t watcher_event[BUTTON_TOTAL_COUNT];

static void badge_button_event_cb_process( badge_button_t button,
                                           badge_button_event_t event
                                         )
{
    // Store the received event.
    watcher_event[(int)button] = event;
}

namespace nb = nsec::button;
namespace ns = nsec::scheduling;
namespace ng = nsec::g;
namespace nbb = nsec::board::button;

nb::watcher::watcher(nb::new_button_event_notifier new_button_notifier) noexcept
    : ns::periodic_task<watcher>(nsec::config::button::polling_period_ms),
      _notify_new_event{new_button_notifier}
{
    start();
}

void nb::watcher::setup() noexcept
{
    // Reset the array used to store the button events.
    for( int i = 0; i < BUTTON_TOTAL_COUNT; i++) {
        watcher_event[i] = BADGE_BUTTON_NO_EVENT;
    }
    
    // Register button callback.
    buttons_register_ca(badge_button_event_cb_process);
}

void nb::watcher::tick(
    [[maybe_unused]] ns::absolute_time_ms current_time_ms) noexcept
{
    // Check the state of all button pins and debounce as needed
      for( int i = 0; i < BUTTON_TOTAL_COUNT; i++) {
        if( watcher_event[i] != BADGE_BUTTON_TOTAL_COUNT) {
            const auto new_event = watcher_event[i];
            
            // Clear recorded event.
            watcher_event[i] = BADGE_BUTTON_NO_EVENT;
        
            // Send the event to the badge notifier.
            _notify_new_event( static_cast<id>(i),
                               static_cast<event>(new_event)
                              );
        }
    }
}
