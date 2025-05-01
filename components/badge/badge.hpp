/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * SPDX-FileCopyrightText: 2025 NorthSec
 */

#ifndef NSEC_RUNTIME_BADGE_HPP
#define NSEC_RUNTIME_BADGE_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <badge-button/watcher.hpp>
#include <badge-led-strip/strip_animator.hpp>
#include <badge-network/network_handler.hpp>
#include <badge-dock/dock_detector.hpp>
#include <cstdint>
#include <utils/logging.hpp>
#include <badge/id.hpp>

namespace nsec::runtime
{

/*
 * Current state of the badge.
 */
class badge
{
  public:
    badge();

    /* Deactivate copy and assignment. */
    badge(const badge &) = delete;
    badge(badge &&) = delete;
    badge &operator=(const badge &) = delete;
    badge &operator=(badge &&) = delete;
    ~badge() = default;

    void start();

    std::uint8_t level() const noexcept;
    void apply_score_change(uint16_t new_badges_discovered_count) noexcept;
    void apply_new_sponsor(uint8_t sponsor_id) noexcept;
    void apply_animation(uint8_t animation_id) noexcept;
    void apply_dock_status(bool detected) noexcept;
    void apply_i2c_command(uint8_t cmd, uint8_t value) noexcept;
    bool is_docked() noexcept;

  private:
    struct eeprom_config {
        uint16_t version_magic;
        uint8_t selected_animation_id;
        uint8_t social_level;
    };

    void load_config();
    void save_config() const;
    void factory_reset();

    // Handle new button event
    void on_button_event(button::id button, button::event event) noexcept;

    enum cycle_animation_direction : int8_t { PREVIOUS = -1, NEXT = 1 };
    void
    _cycle_selected_animation(cycle_animation_direction direction) noexcept;
    void _update_leds(button::id id, button::event event) noexcept;

    void _idle_social_level_and_health(uint8_t state) noexcept;

    void set_social_level(uint8_t new_level, bool save) noexcept;
    void set_sponsor_flag(uint16_t new_level, bool save) noexcept;

    static uint8_t
    _compute_new_social_level(uint8_t current_social_level,
                              uint16_t new_badges_discovered_count) noexcept;
    void _set_selected_animation(uint8_t animation_id, bool save,
                              bool set_idle_animation) noexcept;
    void _led_update_clearance_level();
    uint32_t _check_social_level(uint8_t social_level);
    uint32_t _process_check1(uint8_t social_level);
    uint32_t _process_check2(uint8_t social_level);
    
    void _lcd_display_social_level();
    void _lcd_display_current_animation();
    void _lcd_display_sponsor_count();
    void _lcd_display_update_current_screen();
    void _lcd_display_ir_exchange();

    // Setup hardware.
    void _setup();

    badge_unique_id _get_unique_id();

    mutable SemaphoreHandle_t _public_access_semaphore;
    uint8_t _social_level = 0;
    uint8_t _sponsor_count = 0;
    uint16_t _sponsor_flag = 0;
    uint8_t _clearance_level = 0;
    uint8_t _selected_animation = 0;
    uint8_t _prev_selected_animation = 0;
    uint8_t _idle_press_down_tracking = 1;
    uint8_t _idle_lcd_screen_nb = 0;
    bool _docked = false;

    bool _is_expecting_factory_reset : 1 = 0;
    uint32_t _social_level_check;

    button::watcher _button_watcher;

    nsec::communication::network_handler _network_handler;

    nsec::led::strip_animator _strip_animator;

    nsec::logging::logger _logger;

    nsec::dock::dock_detector _dock_detector;
};
} // namespace nsec::runtime

#endif // NSEC_RUNTIME_BADGE_HPP
