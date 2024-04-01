/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BUTTON_WATCHER_HPP
#define NSEC_BUTTON_WATCHER_HPP

#include "scheduling/task.hpp"

#include <stdint.h>

namespace nsec::button
{

enum class id { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, OK = 4, CANCEL = 5 };
enum class event { SINGLE_CLICK, LONG_PRESS };

using new_button_event_notifier = void (*)(id id, event event);

/*
 * Tracks the state of the badge's buttons to debounce and transform
 * the pin readings into UI button events.
 */
class watcher : public nsec::scheduling::periodic_task<watcher>
{
  public:
    friend class periodic_task<watcher>;

    explicit watcher(new_button_event_notifier new_button_notifier) noexcept;

    // Deactivate copy and assignment.
    watcher(const watcher &) = delete;
    watcher(watcher &&) = delete;
    watcher &operator=(const watcher &) = delete;
    watcher &operator=(watcher &&) = delete;
    ~watcher() = default;

    // Setup hardware.
    void setup() noexcept;

  protected:
    void tick(scheduling::absolute_time_ms current_time_ms) noexcept;

  private:
    new_button_event_notifier _notify_new_event;
};

} // namespace nsec::button

#endif // NSEC_BUTTON_WATCHER_HPP
