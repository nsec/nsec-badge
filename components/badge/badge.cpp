/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#include "badge.hpp"
#include "badge-led-strip/strip_animator.hpp"
#include "badge_nsec_logo.h"
#include "badge_ssd1306_helper.hpp"
#include "utils/lock.hpp"
#include <badge-network/network_handler.hpp>
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

// Select how the clearance level LEDs are mapped.
//#define CLEARANCE_LED_MAP_TO_SOCIAL_LEVEL
#define CLEARANCE_LED_MAP_TO_SPONSOR_COUNT

namespace
{
constexpr uint16_t config_version_magic = 0xBAD8;

#ifdef CLEARANCE_LED_MAP_TO_SOCIAL_LEVEL
unsigned int social_level_to_clearance_led_count(unsigned int level)
{
    // Mapping of the social level to the clearance level
    // - The social level range is 0 to 200.
    // - The clearance range is 1 to 6.
    // - Table field for clearance mapping is the
    //   "Social Level Upper Boundary".
    const std::array<std::uint8_t, 6> clearance_mappings = {1,  6,   17,
                                                            57, 106, 149};

    const auto mapping_it =
        std::upper_bound(clearance_mappings.begin(), clearance_mappings.end(),
                         std::max<std::uint8_t>(level, 1));
    return mapping_it - clearance_mappings.begin();
}
#endif

#ifdef CLEARANCE_LED_MAP_TO_SPONSOR_COUNT
unsigned int sponsor_count_to_clearance_led_count(unsigned int level)
{
    // Mapping of the social level to the clearance level
    // - The sponsor count range is 0 to 14.
    // - The clearance range is 1 to 6.
    // - Table field for clearance mapping is the
    //   "Social Level Upper Boundary".
    const std::array<std::uint8_t, 6> clearance_mappings = {
        1, 3, 5, 8, 11, 14};

    const auto mapping_it =
        std::upper_bound(clearance_mappings.begin(), clearance_mappings.end(),
                         std::max<std::uint8_t>(level, 1));
    return mapping_it - clearance_mappings.begin();
}
#endif

} // anonymous namespace

nr::badge::badge()
    : _button_watcher([](nsec::button::id id, nsec::button::event event) {
          nsec::g::the_badge->on_button_event(id, event);
      }),
      _network_handler(), _logger("badge", nsec::config::logging::badge_level)
{
    _setup();
    _public_access_semaphore = xSemaphoreCreateMutex();
}

void nr::badge::start()
{
    _strip_animator.start();

    // Set initial Clearance level.
    _led_update_clearance_level();
}

void nr::badge::load_config()
{
    nsec::persistence::config_store config_store;

    std::uint8_t social_level = nsec::config::social::initial_level;
    std::uint8_t selected_animation_id = social_level;
    std::uint16_t sponsor_flag = 0;
    std::uint8_t sponsor_count = 0;

    const auto loaded_config = config_store.load();
    if (loaded_config) {
        social_level = loaded_config->social_level;
        selected_animation_id = loaded_config->selected_animation_id;
        sponsor_flag = loaded_config->sponsor_flag;

        // Retreive sponsor counts.
        std::uint16_t tmp_sponsor_flag = sponsor_flag;
        for (unsigned int i = 0; i < 16; i++) {
            if ((tmp_sponsor_flag & 0x0001) == 0x0001) {
                sponsor_count++;
            }
            tmp_sponsor_flag = tmp_sponsor_flag >> 1;
        }

        _logger.info("Found config on storage: social_level={0}, "
                     "selected_animation={1}, sponsor_flag=0x{2:04X}",
                     social_level, selected_animation_id, sponsor_flag);
    } else {
        _logger.info("No config found on storage");
    }

    set_social_level(social_level, false);
    _set_selected_animation(selected_animation_id, false, true);
    set_sponsor_flag(sponsor_flag, false);
    _sponsor_count = sponsor_count;
}

void nr::badge::save_config() const
{
    nsec::persistence::config_store config_store;

    config_store.save_selected_animation_id(_selected_animation);
    config_store.save_social_level(_social_level);
    config_store.save_sponsor_flag(_sponsor_flag);
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

    if (event == nsec::button::event::SINGLE_CLICK) {
        // Send the received event to the LEDs function.
        _update_leds(button, event);
    } else if (event == nsec::button::event::LONG_PRESS) {
        if (button == nsec::button::id::OK) {
            // Send master sync IR ready.
        }
    }
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

void nr::badge::set_sponsor_flag(uint16_t new_level, bool save) noexcept
{
    _sponsor_flag = new_level;
    if (save) {
        save_config();
    }
}

void nr::badge::apply_score_change(
    uint16_t new_badges_discovered_count) noexcept
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
    _led_update_clearance_level();
}

void nr::badge::apply_new_sponsor(uint8_t sponsor_id) noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);

    // Update the sponsor flag with the new ID.
    uint16_t new_sponsor_flag = _sponsor_flag | (1 << (sponsor_id - 1));
    _logger.info("New sponsor ID={0}: old_flag=0x{1:04X}, new_flag=0x{2:04X}",
                 sponsor_id, _sponsor_flag, new_sponsor_flag);

    // If new sponsor detected; increment count
    if (new_sponsor_flag != _sponsor_flag) {
        _sponsor_count = _sponsor_count + 1;
        _logger.info("New sponsor detected: new_count={}", _sponsor_count);

        // Save to configuration
        set_sponsor_flag(new_sponsor_flag, true);

        // Update LCD, if applicable.
        _lcd_display_sponsor_count();

        // Update Clearance level.
        _led_update_clearance_level();
    }
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
}

void nr::badge::_led_update_clearance_level()
{
#if defined(CLEARANCE_LED_MAP_TO_SOCIAL_LEVEL)
    uint8_t new_clearence_level =
        social_level_to_clearance_led_count(_social_level);
#elif defined(CLEARANCE_LED_MAP_TO_SPONSOR_COUNT)
    uint8_t new_clearence_level =
        sponsor_count_to_clearance_led_count(_sponsor_count);
#else
    uint8_t new_clearence_level = 0;
#endif

    // Only update clearance level, if there is a change.
    if (new_clearence_level != _clearance_level) {
        // Update Clearance level (only if changed).
        nsec::g::the_badge->_strip_animator.set_clearance_meter_bar(
            new_clearence_level);

        _logger.info("Clearance level: orig level={}, new level={}",
                     _clearance_level, new_clearence_level);

        _clearance_level = new_clearence_level;
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
    if (_idle_lcd_screen_nb == 1) {
        sprintf(lcd_print, "Animation    %3u", _selected_animation);
        badge_print_text(1, lcd_print, 16, 0);
    }
}

void nr::badge::_lcd_display_sponsor_count()
{
    char lcd_print[17];

    // Display sponsor count on LCD (idle screen nb. 1).
    if (_idle_lcd_screen_nb == 1) {
        sprintf(lcd_print, "Sponsor      %3u", _sponsor_count);
        badge_print_text(2, lcd_print, 16, 0);
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
        _lcd_display_sponsor_count();

        // Setup next entry.
        _idle_press_down_tracking = 0;
    }
}
