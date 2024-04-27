/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * Copyright 2024 NorthSec
 */

/******************************************************************************
 *
 * Description: Badge Button Processing.
 *              - Espressif IOT_BUTTON interface is used to get the
 *                different button events.
 *                1) This interface handle the button debouncing.
 *              - The badge buttons process only handle the "Single Click" &
 *                the "Long Press Event" from the "IOT Button".
 *                1) All other event are ignored.
 *                2) In the current implementation, the "Double Click" &
 *                   "Multiple Click" events is ignored.
 *
 * Process:
 * - Setup all events callback for the differents buttons.
 * - Process all received callback event and mapped to badge button
 *   events (Single Click & Long Press) or ignored the events.
 * - Send the event to the registered callback function
 *   (on the received callback itself).
 *
 * Reference:
 * - Espressif IOT Button documentation & API
 *  https://docs.espressif.com/projects/esp-iot-solution/en/latest/input_device/button.html
 *
 *****************************************************************************/

#include <array>
#include <badge-button/watcher.hpp>
#include <badge/globals.hpp>
#include <esp_log.h>
#include <iot_button.h>
#include <utils/board.hpp>
#include <utils/config.hpp>

namespace nb = nsec::button;
namespace ns = nsec::scheduling;
namespace ng = nsec::g;
namespace nbb = nsec::board::button;

nb::watcher::watcher(nb::new_button_event_notifier new_button_notifier) noexcept
    : _notify_new_event{new_button_notifier}, _logger{"Button watcher"}
{
}

nb::watcher::~watcher()
{
    for (auto &context : _button_callback_contexts) {
        const auto ret = iot_button_delete(context.button_handle);
        if (ret != ESP_OK) {
            _logger.error("Failed to delete button handle: id={}",
                          context.button_id);
        }
    }
}

void nb::watcher::setup()
{
    _logger.info("Setting up button callbacks");

    const std::array<button_event_t, 2> monitored_events = {
        {BUTTON_PRESS_DOWN, BUTTON_LONG_PRESS_HOLD}};

    for (auto &btn_context : _button_callback_contexts) {
        for (auto monitored_event : monitored_events) {
            const auto ret = iot_button_register_cb(
                reinterpret_cast<button_handle_t>(btn_context.button_handle),
                monitored_event, _button_handler, &btn_context);

            _logger.debug("Registered callback for button: native_event={}, "
                          "gpio={}, id={}",
                          monitored_event, btn_context.button_gpio,
                          btn_context.button_id);

            if (ret != ESP_OK) {
                NSEC_THROW_ERROR(fmt::format("Failed to register button event"
                                             "callback: button_id={},"
                                             "gpio = {} ",
                                             btn_context.button_id,
                                             btn_context.button_gpio));
            }
        }
    }
}

void *nb::watcher::_create_button_handle(unsigned int gpio_number)
{
    const button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = std::int32_t(gpio_number),
            .active_level = 0,
#if CONFIG_GPIO_BUTTON_SUPPORT_POWER_SAVE
            .enable_power_save = true,
#endif
        }};

    const auto handle = iot_button_create(&gpio_btn_cfg);
    if (!handle) {
        NSEC_THROW_ERROR("Failed to create button handle");
    }

    return handle;
}

void nb::watcher::_button_handler(void *button_handle, void *opaque_context)
{
    const auto *context =
        static_cast<const button_callback_context *>(opaque_context);

    const auto id = context->button_id;
    const auto native_event = iot_button_get_event(context->button_handle);

    context->watcher_instance->_logger.info(
        "Button handler called: button_id={}, native_event={}", id,
        native_event);

    // Convert ESP-IDF button event into our native events.
    nb::event badge_button_event;
    switch (native_event) {
    case BUTTON_PRESS_DOWN:
        badge_button_event = nb::event::SINGLE_CLICK;
        break;
    case BUTTON_LONG_PRESS_HOLD:
        badge_button_event = nb::event::LONG_PRESS;
        break;
    default:
        NSEC_THROW_ERROR(fmt::format("Unexpected native button event type: "
                                     "button_id={}, event_type={}",
                                     id, native_event));
    }

    // Notify the badge of a new button event.
    context->watcher_instance->_notify_new_event(id, badge_button_event);
}