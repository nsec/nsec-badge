/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_RUNTIME_BADGE_HPP
#define NSEC_RUNTIME_BADGE_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <badge-button/watcher.hpp>
#include <badge-led-strip/strip_animator.hpp>
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

    static uint8_t
    _compute_new_social_level(uint8_t current_social_level,
                              uint16_t new_badges_discovered_count) noexcept;
    void _set_selected_animation(uint8_t animation_id, bool save,
                                 bool set_idle_animation) noexcept;

    void _lcd_display_social_level();
    void _lcd_display_current_animation();
    void _lcd_display_update_current_screen();

    // Setup hardware.
    void _setup();

    badge_unique_id _get_unique_id();

    mutable SemaphoreHandle_t _public_access_semaphore;
    uint8_t _social_level = 0;
    uint8_t _selected_animation = 0;
    uint8_t _idle_press_down_tracking = 1;
    uint8_t _idle_lcd_screen_nb = 0;

    bool _is_expecting_factory_reset : 1 = 0;

    button::watcher _button_watcher;

    nsec::led::strip_animator _strip_animator;

    nsec::logging::logger _logger;
};
} // namespace nsec::runtime

#endif // NSEC_RUNTIME_BADGE_HPP
