/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "badge.hpp"
#include "badge-led-strip/strip_animator.hpp"
#include "badge_ssd1306_helper.hpp"
#include "badge_nsec_logo.h"
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
namespace nb = nsec::button;
namespace nl = nsec::led;
namespace nsync = nsec::synchro;

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
      _logger("badge", nsec::config::logging::badge_level)
{
    _setup();
    _public_access_semaphore = xSemaphoreCreateMutex();
}

void nr::badge::start()
{
    _strip_animator.start();
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

void nr::badge::on_button_event(nsec::button::id button,
                                nsec::button::event event) noexcept
{
    _logger.info("Button event: button={}, event={}", (int)button, (int)event);

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

    // Update Clearance level.
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

    _logger.info("Cycling animation: direction={}, orig ID={}, new ID={}",
                 direction, original_animation_id, new_selected_animation);

    _set_selected_animation(new_selected_animation, true, true);

    // Set the LEDs pattern.
    nsec::g::the_badge->_strip_animator.set_idle_animation(
        new_selected_animation);
}

void nr::badge::_update_leds(nsec::button::id id,
                             nsec::button::event event) noexcept
{
    // Verify the event type.
    if (event == nsec::button::event::SINGLE_CLICK) {
        // Process "SINGLE_CLICK" events.
        switch (id) {
        case nsec::button::id::DOWN:
        case nsec::button::id::UP:
            nsec::g::the_badge->_cycle_selected_animation(
            id == nsec::button::id::DOWN
                ? nsec::runtime::badge::cycle_animation_direction::PREVIOUS
                : nsec::runtime::badge::cycle_animation_direction::NEXT);

            // Update current animation value, on LCD, if applicable.
            _lcd_display_current_animation();
            break;
        case nsec::button::id::OK:
            // Update selected LCD screen to display.
            _lcd_display_update_current_screen();
            break;
        default:
            break;
        }
    } else if(event == nsec::button::event::LONG_PRESS) {
        // Process "LONG_PRESS" events.
        if (id == nsec::button::id::OK) {
            // Send master sync IR ready.
        }
    }
}

void nr::badge::_lcd_display_social_level()
{
    char lcd_print[17];

    // Display current social level on LCD.
    sprintf(lcd_print, "Social Level %3u", _social_level);
    badge_print_text(0, lcd_print, 16, 0);
}

void nr::badge::_lcd_display_current_animation()
{
    char lcd_print[17];

    // Display current animation on LCD (idle screen nb. 1).
    if( _idle_lcd_screen_nb == 1 ) {
        sprintf(lcd_print, "Animation    %3u", _selected_animation);
        badge_print_text(1, lcd_print, 16, 0);
    }
}

void nr::badge::_lcd_display_update_current_screen()
{
    if (_idle_press_down_tracking == 0) {
        // Display Idle LCD Screen 0:
        // * NorthSec logo
        _idle_lcd_screen_nb = 0;
        badge_ssd1306_clear();
        badge_lcd_nsec_logo();

        // Reset press OK tracking.
        _idle_press_down_tracking = 1;
    } else {
        // Display Idle LCD Screen 1:
        // * Current social level.
        // * Selected animation.
        _idle_lcd_screen_nb = 1;
        badge_ssd1306_clear();
        _lcd_display_social_level();
        _lcd_display_current_animation();

       // Setup next entry.
       _idle_press_down_tracking = 0;
    }
}
