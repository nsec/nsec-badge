/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "badge.hpp"
#include "badge-led-strip/strip_animator.hpp"
#include "badge-network/network_messages.hpp"
#include "utils/lock.hpp"
#include <badge-persistence/badge_store.hpp>
#include <badge-persistence/config_store.hpp>
#include <badge-persistence/utils.hpp>
#include <badge/globals.hpp>

#include <algorithm>
#include <array>
#include <cstring>
#include <esp_mac.h>

namespace nr = nsec::runtime;
namespace nc = nsec::communication;
namespace nb = nsec::button;
namespace nl = nsec::led;
namespace ns = nsec::scheduling;
namespace nsync = nsec::synchro;

// Wire message type formatter
template <>
struct fmt::formatter<nc::message::type> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nc::message::type msg_type, FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (msg_type) {
        case nc::message::type::ANNOUNCE_BADGE_ID:
            name = "ANNOUNCE_BADGE_ID";
            break;
        case nc::message::type::PAIRING_ANIMATION_PART_1_DONE:
            name = "PAIRING_ANIMATION_PART_1_DONE";
            break;
        case nc::message::type::PAIRING_ANIMATION_PART_2_DONE:
            name = "PAIRING_ANIMATION_PART_2_DONE";
            break;
        case nc::message::type::PAIRING_ANIMATION_DONE:
            name = "PAIRING_ANIMATION_DONE";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

// Network app state formatter
template <>
struct fmt::formatter<nr::badge::network_app_state>
    : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nr::badge::network_app_state app_state, FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (app_state) {
        case nr::badge::network_app_state::UNCONNECTED:
            name = "UNCONNECTED";
            break;
        case nr::badge::network_app_state::EXCHANGING_IDS:
            name = "EXCHANGING_IDS";
            break;
        case nr::badge::network_app_state::ANIMATE_PAIRING:
            name = "ANIMATE_PAIRING";
            break;
        case nr::badge::network_app_state::ANIMATE_PAIRING_COMPLETED:
            name = "ANIMATE_PAIRING_COMPLETED";
            break;
        case nr::badge::network_app_state::IDLE:
            name = "IDLE";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

// pairing completed animator animation state formatter
template <>
struct fmt::formatter<nr::badge::pairing_completed_animator::animation_state>
    : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(
        nr::badge::pairing_completed_animator::animation_state animation_state,
        FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (animation_state) {
        case nr::badge::pairing_completed_animator::animation_state::
            SHOW_PAIRING_RESULT:
            name = "SHOW_PAIRING_RESULT";
            break;
        case nr::badge::pairing_completed_animator::animation_state::
            SHOW_NEW_LEVEL:
            name = "SHOW_NEW_LEVEL";
            break;
        case nr::badge::pairing_completed_animator::animation_state::
            SHOW_HEALTH:
            name = "SHOW_HEALTH";
            break;
        case nr::badge::pairing_completed_animator::animation_state::DONE:
            name = "DONE";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

// button event formatter
template <> struct fmt::formatter<nb::event> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nb::event button_event, FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (button_event) {
        case nb::event::SINGLE_CLICK:
            name = "SINGLE_CLICK";
            break;
        case nb::event::LONG_PRESS:
            name = "LONG_PRESS";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

namespace
{
constexpr uint16_t config_version_magic = 0xBAD8;

unsigned int social_level_to_health_led_count(unsigned int level)
{
    // Mapping of the social level to the health level
    // - The social level range is 0 to 200.
    // - The health range is 1 to 16.
    // - Table field for health mapping is the
    //   "Social Level Upper Boundary".
    const std::array<std::uint8_t, 16> health_mappings = {
        1, 3, 6, 11, 17, 25, 34, 44, 57, 71, 88, 106, 127, 149, 174, 200};

    const auto mapping_it =
        std::upper_bound(health_mappings.begin(), health_mappings.end(),
                         std::max<std::uint8_t>(level, 1));
    return mapping_it - health_mappings.begin();
}
} // anonymous namespace

nr::badge::badge()
    : _button_watcher([](nsec::button::id id, nsec::button::event event) {
          nsec::g::the_badge->on_button_event(id, event);
      }),
      _network_handler(), _logger("badge")
{
    _setup();
    _public_access_semaphore = xSemaphoreCreateMutex();
    _set_network_app_state(network_app_state::UNCONNECTED);
}

void nr::badge::start()
{
    _strip_animator.start();
    _timer.start();
}

void nr::badge::load_config()
{
    nsec::persistence::config_store config_store;

    std::uint8_t social_level = nsec::config::social::initial_level;
    std::uint8_t selected_animation_id = social_level;

    const auto loaded_config = config_store.load();
    if (loaded_config) {
        social_level = loaded_config->social_level;
        selected_animation_id = loaded_config->selected_animation_id;

        _logger.info(
            "Found config on storage: social_level={}, selected_animation={}",
            social_level, selected_animation_id);
    } else {
        _logger.info("No config found on storage");
    }

    set_social_level(social_level, false);
    _set_selected_animation(selected_animation_id, false, true);
}

void nr::badge::save_config() const
{
    nsec::persistence::config_store config_store;

    config_store.save_selected_animation_id(_selected_animation);
    config_store.save_social_level(_social_level);
}

void nr::badge::factory_reset()
{
    nsec::persistence::utils::reset_storage();

    // Really hackish, but an unhandled exception causes a reset.
    NSEC_THROW_ERROR("Factory reset");
}

void nr::badge::_setup()
{
    _logger.info("Running badge setup");
    _button_watcher.setup();
    load_config();
}

nr::badge_unique_id nr::badge::_get_unique_id()
{
    uint8_t mac_bytes[6] = {0};

    esp_read_mac(mac_bytes, ESP_MAC_EFUSE_FACTORY);

    return nr::badge_unique_id{mac_bytes[0], mac_bytes[1], mac_bytes[2],
                               mac_bytes[3], mac_bytes[4], mac_bytes[5]};
}

uint8_t nr::badge::level() const noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);
    return _social_level;
}

bool nr::badge::is_connected() const noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);
    return _current_network_app_state != network_app_state::UNCONNECTED;
}

void nr::badge::on_button_event(nsec::button::id button,
                                nsec::button::event event) noexcept
{
    _logger.info("Button event: button={}, event={}", button, event);

    if (_current_network_app_state != network_app_state::UNCONNECTED &&
        _current_network_app_state != network_app_state::IDLE) {
        // Don't allow button press during "modal" states.
        _logger.debug("Ignoring button press during modal animation");
        return;
    }

    // Send the received event to the LEDs function.
    _update_leds(button, event);
}

void nr::badge::set_social_level(uint8_t new_level, bool save) noexcept
{
    new_level =
        std::clamp(new_level, uint8_t(0), nsec::config::social::max_level);

    _social_level = new_level;
    if (save) {
        save_config();
    }
}

void nr::badge::on_disconnection() noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);
    _logger.info("Badge network disconnected");
    _set_network_app_state(network_app_state::UNCONNECTED);
}

void nr::badge::on_pairing_begin() noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);
    _logger.info("Network layer pairing begin event");
}

void nr::badge::on_pairing_end(nc::peer_id_t our_peer_id,
                               uint8_t peer_count) noexcept
{
    // The network is up: the application layer takes over to perform the
    // pairing animations.
    nsync::lock_guard lock(_public_access_semaphore);

//    _logger.info("Network layer pairing end event: peer_id={}, peer_count={}",
//                 _network_handler.peer_id(), peer_count);
    _set_network_app_state(network_app_state::ANIMATE_PAIRING);
}

void nr::badge::_set_network_app_state(
    nr::badge::network_app_state new_state) noexcept
{
    _logger.info("Network application state changed: previous={}, new={}",
                 _current_network_app_state, new_state);

    //_id_exchanger.reset();
    _pairing_animator.reset();
    _pairing_completed_animator.reset(*this);

    _current_network_app_state = new_state;
    switch (new_state) {
    case network_app_state::ANIMATE_PAIRING:
        _pairing_animator.start(*this);
        break;
    case network_app_state::EXCHANGING_IDS:
    //    _id_exchanger.start(*this);
        break;
    case network_app_state::ANIMATE_PAIRING_COMPLETED:
        _pairing_completed_animator.start(*this);
        break;
    case network_app_state::IDLE:
    case network_app_state::UNCONNECTED:
        _strip_animator.set_idle_animation(_selected_animation);
        break;
    }
}

nr::badge::badge_discovered_result
nr::badge::on_badge_discovered(const uint8_t *id) noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);

    nsec::runtime::badge_unique_id new_id;
    std::memcpy(new_id.data(), id, new_id.size());

    _logger.info("Badge discovered event: badge_id={}", new_id);

    nsec::persistence::badge_store badge_store;

    for (const auto stored_id : badge_store) {
        if (stored_id == new_id) {
            return badge_discovered_result::ALREADY_KNOWN;
        }
    }

    badge_store.save_id(new_id);
    return badge_discovered_result::NEW;
}

void nr::badge::on_badge_discovery_completed() noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);

    _logger.info("Badge discovery completed");
    //_badges_discovered_last_exchange = _id_exchanger.new_badges_discovered();
    _set_network_app_state(network_app_state::ANIMATE_PAIRING_COMPLETED);
}

nr::badge::pairing_animator::pairing_animator() : _logger("pairing_animator")
{
    _animation_state(animation_state::DONE);
}

// Animation state formatter
template <>
struct fmt::formatter<nr::badge::pairing_animator::animation_state>
    : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(nr::badge::pairing_animator::animation_state animation_state,
                FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (animation_state) {
        case nr::badge::pairing_animator::animation_state::
            WAIT_MESSAGE_ANIMATION_PART_1:
            name = "WAIT_MESSAGE_ANIMATION_PART_1";
            break;
        case nr::badge::pairing_animator::animation_state::LIGHT_UP_UPPER_BAR:
            name = "LIGHT_UP_UPPER_BAR";
            break;
        case nr::badge::pairing_animator::animation_state::LIGHT_UP_LOWER_BAR:
            name = "LIGHT_UP_LOWER_BAR";
            break;
        case nr::badge::pairing_animator::animation_state::
            WAIT_MESSAGE_ANIMATION_PART_2:
            name = "WAIT_MESSAGE_ANIMATION_PART_2";
            break;
        case nr::badge::pairing_animator::animation_state::WAIT_DONE:
            name = "WAIT_DONE";
            break;
        case nr::badge::pairing_animator::animation_state::DONE:
            name = "DONE";
            break;
        default:
            break;
        }

        return fmt::formatter<string_view>::format(name, ctx);
    }
};

void nr::badge::pairing_animator::_animation_state(
    animation_state new_state) noexcept
{
    _logger.info(
        "Transitionning animation state: previous_state={}, new_state={}",
        _current_state, new_state);
    _current_state = new_state;
    _state_counter = 0;
}

nr::badge::pairing_animator::animation_state
nr::badge::pairing_animator::_animation_state() const noexcept
{
    return animation_state(_current_state);
}

void nr::badge::pairing_animator::start(nr::badge &badge) noexcept
{
    _logger.info("Starting animation");

    if (nsec::g::the_badge->_network_handler.position() ==
        nc::network_handler::link_position::LEFT_MOST) {
        _animation_state(animation_state::LIGHT_UP_UPPER_BAR);
    } else {
        _animation_state(animation_state::WAIT_MESSAGE_ANIMATION_PART_1);
    }

    badge._timer.period_ms(
        nsec::config::badge::pairing_animation_time_per_led_progress_bar_ms);
    nsec::g::the_badge->_strip_animator.set_red_to_green_led_progress_bar(
        0, false);
}

void nr::badge::pairing_animator::reset() noexcept
{
    _animation_state(animation_state::DONE);
}

nr::badge::animation_task::animation_task()
    : periodic_task(250), _logger("Animation task")
{
    _logger.info("Starting task");
}

void nr::badge::animation_task::tick(ns::absolute_time_ms current_time_ms)
{
    nsec::g::the_badge->tick(current_time_ms);
}

void nr::badge::pairing_animator::tick(ns::absolute_time_ms current_time_ms)
{
    switch (_animation_state()) {
    case animation_state::DONE:
        if (_state_counter < 8 &&
            nsec::g::the_badge->_network_handler.position() ==
                nc::network_handler::link_position::LEFT_MOST) {
            // Left-most badge waits for the neighbor to transition states.
            _state_counter++;
        } else {
            nsec::g::the_badge->_set_network_app_state(
                nr::badge::network_app_state::EXCHANGING_IDS);
        }
        break;
    case animation_state::WAIT_MESSAGE_ANIMATION_PART_1:
    case animation_state::WAIT_MESSAGE_ANIMATION_PART_2:
    case animation_state::WAIT_DONE:
        break;
    case animation_state::LIGHT_UP_UPPER_BAR:
        nsec::g::the_badge->_strip_animator.set_red_to_green_led_progress_bar(
            _state_counter, false);
        if (_state_counter < 8) {
            _state_counter++;
        } else if (nsec::g::the_badge->_network_handler.position() ==
                   nc::network_handler::link_position::RIGHT_MOST) {
            _animation_state(animation_state::LIGHT_UP_LOWER_BAR);
        } else {
            _logger.debug("Enqueueing message: type={}",
                          nc::message::type::PAIRING_ANIMATION_PART_1_DONE);
//            nsec::g::the_badge->_network_handler.enqueue_app_message(
//                nc::peer_relative_position::RIGHT,
//                uint8_t(nc::message::type::PAIRING_ANIMATION_PART_1_DONE),
//                nullptr);
            _animation_state(animation_state::WAIT_MESSAGE_ANIMATION_PART_2);
        }

        break;
    case animation_state::LIGHT_UP_LOWER_BAR:
        nsec::g::the_badge->_strip_animator.set_red_to_green_led_progress_bar(
            _state_counter + 8, true);

        if (_state_counter < 8) {
            _state_counter++;
        } else if (nsec::g::the_badge->_network_handler.position() ==
                   nc::network_handler::link_position::LEFT_MOST) {
            _logger.debug("Enqueueing message: type={}",
                          nc::message::type::PAIRING_ANIMATION_DONE);

            nsec::g::the_badge->_set_network_app_state(
                nr::badge::network_app_state::EXCHANGING_IDS);
//            nsec::g::the_badge->_network_handler.enqueue_app_message(
//                nc::peer_relative_position::RIGHT,
//                uint8_t(nc::message::type::PAIRING_ANIMATION_DONE), nullptr);

            _animation_state(animation_state::DONE);
        } else {
            _logger.debug("Enqueueing message: type={}",
                          nc::message::type::PAIRING_ANIMATION_PART_2_DONE);
//            nsec::g::the_badge->_network_handler.enqueue_app_message(
//                nc::peer_relative_position::LEFT,
//                uint8_t(nc::message::type::PAIRING_ANIMATION_PART_2_DONE),
//                nullptr);
            _animation_state(animation_state::WAIT_DONE);
        }

        break;
    }
}

void nr::badge::pairing_animator::new_message(nr::badge &badge,
                                              nc::message::type msg_type,
                                              const uint8_t *payload) noexcept
{
    switch (msg_type) {
    case nc::message::type::PAIRING_ANIMATION_PART_1_DONE:
        _animation_state(animation_state::LIGHT_UP_UPPER_BAR);
        break;
    case nc::message::type::PAIRING_ANIMATION_PART_2_DONE:
        _animation_state(animation_state::LIGHT_UP_LOWER_BAR);
        break;
    case nc::message::type::PAIRING_ANIMATION_DONE:
        if (nsec::g::the_badge->_network_handler.position() !=
            nc::network_handler::link_position::RIGHT_MOST) {
            _logger.debug("Enqueueing message: type={}",
                          nc::message::type::PAIRING_ANIMATION_DONE);

//            nsec::g::the_badge->_network_handler.enqueue_app_message(
//                nc::peer_relative_position::RIGHT,
//                uint8_t(nc::message::type::PAIRING_ANIMATION_DONE), nullptr);
        }

        _animation_state(animation_state::DONE);
        break;
    default:
        _logger.error("Unknown message received by pairing animator");
        break;
    }
}

void nr::badge::tick(ns::absolute_time_ms current_time_ms)
{
    switch (_current_network_app_state) {
    case network_app_state::ANIMATE_PAIRING:
        _pairing_animator.tick(current_time_ms);
        break;
    case network_app_state::ANIMATE_PAIRING_COMPLETED:
        _pairing_completed_animator.tick(*this, current_time_ms);
        break;
    default:
        break;
    }
}

void nr::badge::pairing_completed_animator::start(nr::badge &badge) noexcept
{
    _logger.info("Starting animation");

    badge._timer.period_ms(500);
    memset(current_message, 0, sizeof(current_message));
    _animation_state(badge, animation_state::SHOW_PAIRING_RESULT);
}

void nr::badge::pairing_completed_animator::reset(nr::badge &badge) noexcept
{
    _current_state = animation_state::DONE;
    _state_counter = 0;
}

void nr::badge::pairing_completed_animator::tick(
    nr::badge &badge, ns::absolute_time_ms current_time_ms)
{
    switch (_animation_state()) {
    case animation_state::SHOW_PAIRING_RESULT: {
        if (_state_counter == 0) {
            // Store the new social level.
            badge.apply_score_change(badge._badges_discovered_last_exchange);
	}

        if (_state_counter < 8) {
            // Keep on showing the pairing result animation.
            break;
        }

        std::memset(current_message, 0, sizeof(current_message));

        // Transition to showing the new social level.
        _animation_state(badge, animation_state::SHOW_NEW_LEVEL);
        break;
    }
    case animation_state::SHOW_NEW_LEVEL:
        if (_state_counter < 8) {
            // Keep on showing the new level animation.
            break;
        }

        // Transition to showing the new health status
        _animation_state(badge, animation_state::SHOW_HEALTH);
        break;
    case animation_state::SHOW_HEALTH:
        if (_state_counter < 8) {
            // Keep on showing the current health.
            break;
        }

        // Go back to the idle animation state.
        _animation_state(badge, animation_state::DONE);
        break;
    case animation_state::DONE:
        _logger.warn("Animator still active after reaching the DONE state");
        break;
    }

    _state_counter++;
}

void nr::badge::pairing_completed_animator::_animation_state(
    nr::badge &badge,
    nr::badge::pairing_completed_animator::animation_state new_state)
{
    _logger.info("Animation state change: current={}, new={}", _current_state,
                 new_state);
    _current_state = new_state;
    _state_counter = 0;

    switch (new_state) {
    case nr::badge::pairing_completed_animator::animation_state::
        SHOW_PAIRING_RESULT:
        badge._strip_animator.set_pairing_completed_animation(
            badge._badges_discovered_last_exchange > 0
                ? nl::strip_animator::pairing_completed_animation_type::
                      HAPPY_CLOWN_BARF
                : nl::strip_animator::pairing_completed_animation_type::
                      NO_NEW_FRIENDS);
        break;
    case nr::badge::pairing_completed_animator::animation_state::
        SHOW_NEW_LEVEL: {
        badge._strip_animator.set_show_level_animation(
            badge._badges_discovered_last_exchange > 0
                ? nl::strip_animator::pairing_completed_animation_type::
                      HAPPY_CLOWN_BARF
                : nl::strip_animator::pairing_completed_animation_type::
                      NO_NEW_FRIENDS,
            badge._social_level, false);
        break;
    }
    case nr::badge::pairing_completed_animator::animation_state::SHOW_HEALTH: {
        badge._strip_animator.set_health_meter_bar(
            social_level_to_health_led_count(badge._social_level));
        break;
    }
    case nr::badge::pairing_completed_animator::animation_state::DONE:
        badge._set_network_app_state(network_app_state::IDLE);
        break;
    default:
        break;
    }
}

nr::badge::pairing_completed_animator::animation_state
nr::badge::pairing_completed_animator::_animation_state() const noexcept
{
    return _current_state;
}

void nr::badge::clear_leds()
{
    nsync::lock_guard lock(_public_access_semaphore);
    _strip_animator.set_blank_animation();
}

void nr::badge::apply_score_change(uint16_t new_badges_discovered_count) noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);

    const auto new_social_level =
        _compute_new_social_level(_social_level, new_badges_discovered_count);
    _logger.info("Applying score change: new_badges_discovered_count={}, "
                 "new_social_level={}",
                 new_badges_discovered_count, new_social_level);

    // Saves to configuration
    set_social_level(new_social_level, true);
    _set_selected_animation(_social_level, true, false);
}

uint8_t nr::badge::_compute_new_social_level(
    uint8_t current_social_level, uint16_t new_badges_discovered_count) noexcept
{
    uint16_t new_social_level = current_social_level;
    uint16_t level_up = new_badges_discovered_count;

    if (new_badges_discovered_count > 1) {
        level_up = new_badges_discovered_count *
                   nsec::config::social::
                       multiple_badges_discovered_simultaneously_multiplier;
    }

    new_social_level += level_up;

    return std::clamp<std::uint8_t>(new_social_level, 0,
                                    nsec::config::social::max_level);
}

void nr::badge::_set_selected_animation(uint8_t animation_id,
                                        bool save_to_config,
                                        bool set_idle_animation) noexcept
{
    _selected_animation = animation_id;
    if (set_idle_animation) {
        _strip_animator.set_idle_animation(animation_id);
    }

    if (save_to_config) {
        save_config();
    }
}

void nr::badge::_cycle_selected_animation(
    nr::badge::cycle_animation_direction direction) noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);
    const auto original_animation_id = _selected_animation;

    const auto new_selected_animation = std::clamp<int>(
        _selected_animation + int8_t(direction), 0, _social_level);

    _logger.info("Cycling selected animation: direction={}, "
                 "original_animation_id={}, new_animation_id={}",
                 direction, original_animation_id, new_selected_animation);

    _set_selected_animation(new_selected_animation, true, true);

    // Set the LEDs pattern.
    nsec::g::the_badge->_strip_animator.set_idle_animation(
        new_selected_animation);
}

void nr::badge::_update_leds(nsec::button::id id,
                             nsec::button::event event) noexcept
{
    // Only process "SINGLE_CLICK" events.
    if (event != nsec::button::event::SINGLE_CLICK) {
        return;
    }

    switch (id) {
    case nsec::button::id::LEFT:
    case nsec::button::id::RIGHT:
        nsec::g::the_badge->_cycle_selected_animation(
        id == nsec::button::id::LEFT
            ? nsec::runtime::badge::cycle_animation_direction::PREVIOUS
            : nsec::runtime::badge::cycle_animation_direction::NEXT);

        // Reset press down tracking.
        _idle_press_down_tracking = 0;
        break;
    case nsec::button::id::DOWN:
        if (_idle_press_down_tracking == 0) {
            // Display the Health level on the LEDs.
            _strip_animator.set_health_meter_bar(
                social_level_to_health_led_count(_social_level));

            // Reset press down tracking.
            _idle_press_down_tracking = 1;
        } else {
            // Display the social level on the LEDs.
            _strip_animator.set_show_level_animation(
                nsec::led::strip_animator::pairing_completed_animation_type::
                    IDLE_SOCIAL_LEVEL,
                level(), false);

            // Setup next entry.
            _idle_press_down_tracking = 0;
        }
        break;
    default:
        break;
    }
}
