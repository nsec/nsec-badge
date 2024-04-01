/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include <badge/globals.hpp>
#include <badge-button/watcher.hpp>
#include <utils/board.hpp>
#include <utils/config.hpp>
#include <esp_log.h>
#include "buttons.h"

/* Local debugging options. */
//#define DEBUG_WATCHER_BUTTON_CALLBACK
//#define DEBUG_WATCHER_TASK_BUTTON_CALLBACK

static badge_button_event_t watcher_event[BUTTON_TOTAL_COUNT];

#if defined(DEBUG_WATCHER_BUTTON_CALLBACK) || defined(DEBUG_WATCHER_TASK_BUTTON_CALLBACK)
const char *watcher_button_label_table[] = {
    "UP",
    "DOWN",
    "LEFT",    
    "RIGHT",
    "OK",
    "CANCEL"
};

const char *watcher_button_event_table[] = {
    "SINGLE_CLICK",
    "LONG_PRESS"
};
#endif

static void badge_button_event_cb_process( badge_button_t button,
                                           badge_button_event_t event
                                         )
{
    // Store the received event.
    watcher_event[(int)button] = event;

    #ifdef DEBUG_WATCHER_BUTTON_CALLBACK
    /* Log button event on serial console. */
    ESP_LOGI( "WATCHER BUTTON EVENT", "%s: %s\n",
              watcher_button_label_table[button],
              watcher_button_event_table[event]
            );
    #endif
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

            #ifdef DEBUG_WATCHER_TASK_BUTTON_CALLBACK
            /* Log button event on serial console. */
            ESP_LOGI( "WATCHER TASK BUTTON EVENT", "%s: %s\n",
                      watcher_button_label_table[i],
                      watcher_button_event_table[(int)new_event]
                    );
            #endif
        }
    }
}
