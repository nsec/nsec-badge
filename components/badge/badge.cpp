/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * SPDX-FileCopyrightText: 2025 NorthSec
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
// #define CLEARANCE_LED_MAP_TO_SOCIAL_LEVEL
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
    const std::array<std::uint8_t, 6> clearance_mappings = {1, 3, 5, 8, 11, 14};

    const auto mapping_it =
        std::upper_bound(clearance_mappings.begin(), clearance_mappings.end(),
                         std::max<std::uint8_t>(level, 1));
    return mapping_it - clearance_mappings.begin();
}
#endif

}

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
    _dock_detector.start();

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
    std::uint32_t nvs_social_check;

    const auto loaded_config = config_store.load();
    if (loaded_config) {
        social_level = loaded_config->social_level;
        selected_animation_id = loaded_config->selected_animation_id;
        sponsor_flag = loaded_config->sponsor_flag;
        nvs_social_check = loaded_config->social_level_check;

        // Retreive sponsor counts.
        std::uint16_t tmp_sponsor_flag = sponsor_flag;
        for (unsigned int i = 0; i < 16; i++) {
            if ((tmp_sponsor_flag & 0x0001) == 0x0001) {
                sponsor_count++;
            }
            tmp_sponsor_flag = tmp_sponsor_flag >> 1;
        }

        _logger.info("Found config on storage:");
        _logger.info("- social_level={0}, social_level_check=0x{1:08X}",
                     social_level, nvs_social_check);
        _logger.info("- selected_animation={0}, sponsor_flag=0x{1:04X}",
                     selected_animation_id, sponsor_flag);

        // Check social level.
        if (_check_social_level(social_level) != nvs_social_check) {
            _logger.info("'social level check' error");
            social_level = nsec::config::social::initial_level;
            selected_animation_id = social_level;
        };
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
    config_store.save_social_level_check(
        nsec::g::the_badge->_check_social_level(_social_level));
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

nr::badge_unique_id nr::badge::get_unique_id()
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
        // Prevent updating the LCD display (and animation selection)
        // during the IR synchronization process.
        if (_network_handler.get_ir_protocol_state() ==
            nc::ir_protocol_state::IDLE) {
            // Send the received event to the LEDs function.
            _update_leds(button, event);
        }
    } else if (event == nsec::button::event::LONG_PRESS && button == nsec::button::id::OK) {
        if (_network_handler.get_ir_protocol_state() == nc::ir_protocol_state::IDLE) {
            _lcd_display_ir_exchange();
            _network_handler.start_ir_key_exchange();
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
    uint8_t _idle_lcd_screen_nb_backup = _idle_lcd_screen_nb;

    if (new_badges_discovered_count > 0) {
        const auto new_social_level = _compute_new_social_level(
            _social_level, new_badges_discovered_count);

        _logger.info("Applying score change: new_badges_discovered_count={}, "
                     "new_social_level={}",
                     new_badges_discovered_count, new_social_level);

        // Saves to configuration
        set_social_level(new_social_level, true);
    }

    // Always display the sync status, unless badge dock.
    // * Note: the idle screen is restore by the "Network Handler" process.
    if (!_docked) {
         badge_ssd1306_clear();

        // For the display of the social level.
        _idle_lcd_screen_nb = 1;
        _lcd_display_social_level();
        _idle_lcd_screen_nb = _idle_lcd_screen_nb_backup;

        // Display the sync result.
        badge_print_text(
            2, (new_badges_discovered_count > 0) ? "New badge" : "No new badge",
            12, 0);
        badge_print_text(3, "discovered", 10, 0);
    }

    // Update Clearance level.
    _led_update_clearance_level();
}

void nr::badge::apply_new_sponsor(uint8_t sponsor_id) noexcept
{
    nsync::lock_guard lock(_public_access_semaphore);

    // Only accept ID between 1 & 14.
    if( (sponsor_id == 0) || (sponsor_id > 14)) {
        _logger.info("Received invalid sponsor ID={}", sponsor_id);

        // Ignore the request.
        return;
    }

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

void nr::badge::apply_animation(uint8_t animation_id) noexcept
{
    _set_selected_animation(animation_id, false, true);
}

void nr::badge::apply_dock_status(bool detected) noexcept
{
    _docked = detected;
    if(_docked){
        _prev_selected_animation = _selected_animation;
        badge_ssd1306_clear();
        vTaskDelay(500/portTICK_PERIOD_MS);
        badge_ssd1306_deinit();
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    else{
        apply_animation(_prev_selected_animation);
        vTaskDelay(500/portTICK_PERIOD_MS);
        _idle_press_down_tracking = 0;
        _lcd_display_update_current_screen();
    }
}

void nr::badge::apply_i2c_command(uint8_t cmd, uint8_t value) noexcept
{
    switch(cmd){
        case nsec::config::i2c::sponsor_cmd:
            apply_new_sponsor(value);
            break;
        case nsec::config::i2c::animation_cmd:
            apply_animation(value);
            break;
    }
}

bool nr::badge::is_docked() noexcept
{
    return _docked;
}

uint8_t nr::badge::_compute_new_social_level(
    uint8_t current_social_level, uint16_t new_badges_discovered_count) noexcept
{
    uint8_t new_social_level = current_social_level;
    uint8_t level_up = 0;

    // New badge count is always 1 for the 2025 badge.
    if (new_badges_discovered_count == 1) {
        if (current_social_level <= 20) {
            level_up = 5;
        } else if (current_social_level <= 75) {
            level_up = 4;
        } else if (current_social_level <= 125) {
            level_up = 3;
        } else {
            level_up = 2;
        }
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

uint32_t nr::badge::_check_social_level(uint8_t social_level)
{
    uint32_t check1;
    uint32_t check2;

    nr::badge_unique_id mac = get_unique_id();

    check1 = _process_check1(social_level);
    check2 = _process_check2(social_level);
    check1 = check1 + check2 + (check2 << 15) + (mac[5] & 0xFC);

    return check1;
}

uint32_t nr::badge::_process_check1(uint8_t social_level)
{
    uint32_t check;
    nr::badge_unique_id mac = get_unique_id();

    check = (mac[5] << 8) + mac[3];
    check = check + ((social_level + 51) << 8) + (social_level & 0xFE);

    return check;
}

uint32_t nr::badge::_process_check2(uint8_t social_level)
{
    uint32_t check;
    nr::badge_unique_id mac = get_unique_id();

    check = (mac[3] << 8) + mac[4] + ((uint32_t)social_level << 4);
    check = check + config_version_magic + (mac[5] * (social_level + 3));

    return check;
}

void nr::badge::_lcd_display_social_level()
{
    char lcd_print[17];

    // Display current social level on LCD (idle screen nb. 1).
    if (_idle_lcd_screen_nb == 1 && !_docked) {
        sprintf(lcd_print, "Social Level %3u", _social_level);
        badge_print_text(0, lcd_print, 16, 0);
    }
}

void nr::badge::_lcd_display_current_animation()
{
    char lcd_print[17];

    // Display current animation on LCD (idle screen nb. 1).
    if (_idle_lcd_screen_nb == 1 && !_docked) {
        sprintf(lcd_print, "Animation    %3u", _selected_animation);
        badge_print_text(1, lcd_print, 16, 0);
    }
}

void nr::badge::_lcd_display_sponsor_count()
{
    char lcd_print[17];

    // Display sponsor count on LCD (idle screen nb. 1).
    if (_idle_lcd_screen_nb == 1 && !_docked) {
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

void nr::badge::_lcd_display_ir_exchange()
{
    char lcd_print[17];

    // Display the IR Exchange screen.
    // * Status is updated by "lcd_display_ir_exchange_status" function.
    badge_ssd1306_clear();
    sprintf(lcd_print, "IR Exchange");
    badge_print_text(0, lcd_print, 11, 0);
}

void nr::badge::lcd_display_ir_exchange_status(
    nc::ir_protocol_state state) noexcept
{
    badge_ssd1306_clear();

    switch (state) {
    case nc::ir_protocol_state::IDLE:
        _lcd_display_update_current_screen();
        break;
    case nc::ir_protocol_state::WAITING_FOR_PEER:
        badge_print_text(0, "IR Exchange", 11, 0);
        badge_print_text(1, "Looking for peer", 16, 0);
        break;
    case nc::ir_protocol_state::SENDER:
        badge_print_text(0, "IR Exchange", 11, 0);
        badge_print_text(1, "Sending data...", 15, 0);
        break;
    case nc::ir_protocol_state::RECEIVER:
        badge_print_text(0, "IR Exchange", 11, 0);
        badge_print_text(1, "Receiving data...", 17, 0);
        break;
    case nc::ir_protocol_state::COMPLETED:
        badge_print_text(0, "IR Exchange", 11, 0);
        badge_print_text(1, "Complete!", 9, 0);
        break;
    case nc::ir_protocol_state::TIMEOUT:
        badge_print_text(0, "IR Exchange", 11, 0);
        badge_print_text(1, "Timed out", 9, 0);
        break;
    default:
        break;
    }
}
