/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_BUTTON_WATCHER_HPP
#define NSEC_BUTTON_WATCHER_HPP

#include "scheduling/task.hpp"
#include <array>
#include <fmt/format.h>
#include <stdint.h>
#include <utils/board.hpp>
#include <utils/logging.hpp>

namespace nsec::button
{

enum class id { UP, DOWN, LEFT, RIGHT, OK, CANCEL };
enum class event { SINGLE_CLICK, LONG_PRESS };

using new_button_event_notifier = void (*)(id id, event event);

/*
 * Tracks the state of the badge's buttons to debounce and transform
 * the pin readings into UI button events.
 */
class watcher
{
  public:
    explicit watcher(new_button_event_notifier new_button_notifier) noexcept;

    // Deactivate copy and assignment.
    watcher(const watcher &) = delete;
    watcher(watcher &&) = delete;
    watcher &operator=(const watcher &) = delete;
    watcher &operator=(watcher &&) = delete;
    ~watcher() = default;

    // Setup hardware.
    void setup();

  private:
    struct button_callback_context {
        const watcher *const watcher_instance;
        const nsec::button::id button_id;
        const unsigned int button_gpio;
        void *const button_handle;
    };

    void *_create_button_handle(unsigned int gpio_number);
    static void _button_handler(void *button_handle, void *context);

    std::array<button_callback_context, 6> _button_callback_contexts = {{
        [std::size_t(nsec::button::id::UP)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::UP,
             .button_gpio = nsec::board::button::up,
             .button_handle = _create_button_handle(nsec::board::button::up)},
        [std::size_t(nsec::button::id::DOWN)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::DOWN,
             .button_gpio = nsec::board::button::down,
             .button_handle = _create_button_handle(nsec::board::button::down)},
        [std::size_t(nsec::button::id::LEFT)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::LEFT,
             .button_gpio = nsec::board::button::left,
             .button_handle = _create_button_handle(nsec::board::button::left)},
        [std::size_t(nsec::button::id::RIGHT)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::RIGHT,
             .button_gpio = nsec::board::button::right,
             .button_handle =
                 _create_button_handle(nsec::board::button::right)},
        [std::size_t(nsec::button::id::OK)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::OK,
             .button_gpio = nsec::board::button::ok,
             .button_handle = _create_button_handle(nsec::board::button::ok)},
        [std::size_t(nsec::button::id::CANCEL)] =
            {.watcher_instance = this,
             .button_id = nsec::button::id::CANCEL,
             .button_gpio = nsec::board::button::cancel,
             .button_handle =
                 _create_button_handle(nsec::board::button::cancel)},
    }};
    new_button_event_notifier _notify_new_event;
    nsec::logging::logger _logger;
};

} // namespace nsec::button

// nsec::button::id formatter
template <>
struct fmt::formatter<nsec::button::id> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nsec::button::id button_id, FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (button_id) {
        case nsec::button::id::UP:
            name = "UP";
            break;
        case nsec::button::id::DOWN:
            name = "DOWN";
            break;
        case nsec::button::id::LEFT:
            name = "LEFT";
            break;
        case nsec::button::id::RIGHT:
            name = "RIGHT";
            break;
        case nsec::button::id::OK:
            name = "OK";
            break;
        case nsec::button::id::CANCEL:
            name = "CANCEL";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

#endif // NSEC_BUTTON_WATCHER_HPP
