/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023-2024 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_RUNTIME_BADGE_HPP
#define NSEC_RUNTIME_BADGE_HPP

#include <badge-network/network_handler.hpp>
#include <button/watcher.hpp>
#include <cstdint>

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

    // Setup hardware.
    void setup();

    void relase_focus_current_screen() noexcept;
    void on_splash_complete() noexcept;
    std::uint8_t level() const noexcept;
    bool is_connected() const noexcept;

    void on_disconnection() noexcept;
    void on_pairing_begin() noexcept;
    void on_pairing_end(nsec::communication::peer_id_t our_peer_id,
                        uint8_t peer_count) noexcept;
    nsec::communication::network_handler::application_message_action
    on_message_received(communication::message::type message_type,
                        const uint8_t *message) noexcept;
    void on_app_message_sent() noexcept;

    void apply_score_change(uint8_t new_badges_discovered_count) noexcept;
    void show_badge_info() noexcept;

    void tick(nsec::scheduling::absolute_time_ms current_time_ms) noexcept;

    enum cycle_animation_direction : int8_t { PREVIOUS = -1, NEXT = 1 };
    void cycle_selected_animation(cycle_animation_direction direction) noexcept;

  private:
    enum class network_app_state : uint8_t {
        UNCONNECTED,
        EXCHANGING_IDS,
        ANIMATE_PAIRING,
        ANIMATE_PAIRING_COMPLETED,
        IDLE,
    };
    class network_id_exchanger
    {
      public:
        network_id_exchanger() = default;

        network_id_exchanger(const network_id_exchanger &) = delete;
        network_id_exchanger(network_id_exchanger &&) = delete;
        network_id_exchanger &operator=(const network_id_exchanger &) = delete;
        network_id_exchanger &operator=(network_id_exchanger &&) = delete;
        ~network_id_exchanger() = default;

        void start(badge &) noexcept;
        void new_message(badge &badge,
                         nsec::communication::message::type msg_type,
                         const uint8_t *payload) noexcept;
        void message_sent(badge &badge) noexcept;
        void reset() noexcept;
        uint8_t new_badges_discovered() const noexcept
        {
            return uint8_t(_new_badges_discovered);
        }

      private:
        uint8_t _new_badges_discovered : 5;
        uint8_t _message_received_count : 5;
        bool _send_ours_on_next_send_complete : 1;
        uint8_t _direction : 1;
        bool _done_after_sending_ours : 1;
    };

    class pairing_animator
    {
      public:
        pairing_animator();

        pairing_animator(const pairing_animator &) = delete;
        pairing_animator(pairing_animator &&) = delete;
        pairing_animator &operator=(const pairing_animator &) = delete;
        pairing_animator &operator=(pairing_animator &&) = delete;
        ~pairing_animator() = default;

        void start(badge &) noexcept;
        void new_message(badge &badge,
                         nsec::communication::message::type msg_type,
                         const uint8_t *payload) noexcept;
        void message_sent(badge &badge) noexcept;
        void reset() noexcept;

        void tick(nsec::scheduling::absolute_time_ms current_time_ms) noexcept;

      private:
        enum class animation_state : uint8_t {
            WAIT_MESSAGE_ANIMATION_PART_1,
            LIGHT_UP_UPPER_BAR,
            LIGHT_UP_LOWER_BAR,
            WAIT_MESSAGE_ANIMATION_PART_2,
            WAIT_DONE,
            DONE,
        };

        void _animation_state(animation_state) noexcept;
        animation_state _animation_state() const noexcept;

        uint8_t _current_state : 3;
        uint8_t _state_counter : 5;
    };

    class pairing_completed_animator
    {
      public:
        pairing_completed_animator() = default;

        pairing_completed_animator(const pairing_completed_animator &) = delete;
        pairing_completed_animator(pairing_completed_animator &&) = delete;
        pairing_completed_animator &
        operator=(const pairing_completed_animator &) = delete;
        pairing_completed_animator &
        operator=(pairing_completed_animator &&) = delete;
        ~pairing_completed_animator() = default;

        void start(badge &) noexcept;
        void reset() noexcept;
        void tick(badge &,
                  nsec::scheduling::absolute_time_ms current_time_ms) noexcept;

      private:
        enum class animation_state : uint8_t {
            SHOW_PAIRING_COMPLETE_MESSAGE,
            SHOW_NEW_LEVEL,
        };

        void _animation_state(animation_state) noexcept;
        animation_state _animation_state() const noexcept;

        uint8_t _current_state : 3;
        uint8_t _state_counter : 5;
        char current_message[32];
    };

    class animation_task : public nsec::scheduling::periodic_task<animation_task>
    {
      public:
        explicit animation_task();
        void tick(nsec::scheduling::absolute_time_ms current_time_ms) noexcept;
    };

    struct eeprom_config {
        uint16_t version_magic;
        uint8_t favorite_animation_id;
        bool is_name_set;
        uint8_t social_level;
        char name[nsec::config::user::name_max_length];
    };

    void load_config();
    void save_config() const;
    void factory_reset();

    // Handle new button event
    void on_button_event(button::id button, button::event event) noexcept;
    void set_social_level(uint8_t new_level, bool save) noexcept;

    // Handle network events
    enum class badge_discovered_result : uint8_t { NEW, ALREADY_KNOWN };
    badge_discovered_result on_badge_discovered(const uint8_t *id) noexcept;
    void on_badge_discovery_completed() noexcept;

    network_app_state _network_app_state() const noexcept;
    void _network_app_state(network_app_state) noexcept;

    void _set_user_name_scroll_screen() noexcept;

    static uint8_t
    _compute_new_social_level(uint8_t current_social_level,
                              uint8_t new_badges_discovered_count) noexcept;
    void _set_selected_animation(uint8_t animation_id, bool save) noexcept;

    uint8_t _social_level;
    uint8_t _selected_animation;
    // Storage for network_app_state
    uint8_t _current_network_app_state : 4;
    uint8_t _badges_discovered_last_exchange : 5;
    bool _is_user_name_set : 1;
    bool _is_expecting_factory_reset : 1;
    // Mask to prevent repeats after a screen transition, one bit per button.
    uint8_t _button_had_non_repeat_event_since_screen_focus_change;
    char _user_name[nsec::config::user::name_max_length];

    button::watcher _button_watcher;

    // network
    communication::network_handler _network_handler;
    network_id_exchanger _id_exchanger;
    pairing_animator _pairing_animator;
    pairing_completed_animator _pairing_completed_animator;

    // animation timer
    animation_task _timer;
};
} // namespace nsec::runtime

#endif // NSEC_RUNTIME_BADGE_HPP