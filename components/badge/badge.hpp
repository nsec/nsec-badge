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
#include <badge-network/network_handler.hpp>
#include "badge-network/network_messages.hpp"
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
    bool is_connected() const noexcept;

    void on_disconnection() noexcept;
    void on_pairing_begin() noexcept;
    void on_pairing_end(nsec::communication::peer_id_t our_peer_id,
                        uint8_t peer_count) noexcept;
//    nsec::communication::network_handler::application_message_action
//    on_message_received(communication::message::type message_type,
//                        const uint8_t *message) noexcept;
//    void on_app_message_sent() noexcept;

    void apply_score_change(uint16_t new_badges_discovered_count) noexcept;

    void tick(nsec::scheduling::absolute_time_ms current_time_ms);

    void clear_leds();

    /*
     * Once the network layer has established a connection, "on pairing end" is
     * invoked. This is when the badges start animating the user-visible
     * "pairing" animations and exchanging IDs.
     *
     * When this occurs, the application network layer transitions from
     * the UNCONNECTED to the ANIMATE_PAIRING state.
     *
     * During the ANIMATE_PAIRING state, the "pairing_animator" takes over the
     * actions of the badge. It drives the progress bar forward and sends
     * the PAIRING_ANIMATION_PART_1_DONE once it has lit up the top row of LEDs.
     * It then waits to receive the PAIRING_ANIMATION_PART_2_DONE from its
     * right-side peer.
     *
     * When it receives PAIRING_ANIMATION_PART_2_DONE (or if it is the
     * right-most peer), it starts animating the bottom row of LEDs. Once the
     * bottom-row LED animation has completed, it forwards the
     * PAIRING_ANIMATION_PART_2_DONE message to its left-side peer. It then
     * enters the EXCHANGING_IDS state.
     *
     * In the EXCHANGING_IDS, the network_id_exchanger takes over the control
     * of the badge. The left-most badge starts the exchange by sending an
     * ANNOUNCE_BADGE_ID message. This message contains the badge's peer_id (in
     * the current badge chain) and it's unique ID.
     *
     * This message is forwarded to the right by the various badges in the
     * chain.
     */
    enum class network_app_state : uint8_t {
        UNCONNECTED,
        EXCHANGING_IDS,
        ANIMATE_PAIRING,
        ANIMATE_PAIRING_COMPLETED,
        IDLE,
    };

    class pairing_animator
    {
      public:
        enum class animation_state : uint8_t {
            WAIT_MESSAGE_ANIMATION_PART_1,
            LIGHT_UP_UPPER_BAR,
            LIGHT_UP_LOWER_BAR,
            WAIT_MESSAGE_ANIMATION_PART_2,
            WAIT_DONE,
            DONE,
        };

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

        void tick(nsec::scheduling::absolute_time_ms current_time_ms);

      private:
        void _animation_state(animation_state) noexcept;
        animation_state _animation_state() const noexcept;

        animation_state _current_state;
        uint8_t _state_counter;
        nsec::logging::logger _logger;
    };

    class pairing_completed_animator
    {
      public:
        enum class animation_state : uint8_t {
            SHOW_PAIRING_RESULT,
            SHOW_NEW_LEVEL,
            SHOW_HEALTH,
            DONE,
        };

        pairing_completed_animator() : _logger("pairing_completed_animator")
        {
        }

        pairing_completed_animator(const pairing_completed_animator &) = delete;
        pairing_completed_animator(pairing_completed_animator &&) = delete;
        pairing_completed_animator &
        operator=(const pairing_completed_animator &) = delete;
        pairing_completed_animator &
        operator=(pairing_completed_animator &&) = delete;
        ~pairing_completed_animator() = default;

        void start(badge &) noexcept;
        void reset(badge &) noexcept;
        void tick(badge &, nsec::scheduling::absolute_time_ms current_time_ms);

      private:
        void _animation_state(badge &, animation_state new_state);
        animation_state _animation_state() const noexcept;

        animation_state _current_state;
        uint8_t _state_counter;
        char current_message[32];
        nsec::logging::logger _logger;
    };

  private:
#if 0
    class network_id_exchanger
    {
      public:
        network_id_exchanger() : _logger("network_id_exchanger")
        {
        }

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
        uint16_t new_badges_discovered() const noexcept
        {
            return _new_badges_discovered;
        }

      private:
        uint16_t _new_badges_discovered;
        uint8_t _message_received_count;
        bool _send_ours_on_next_send_complete;
        uint8_t _direction;
        bool _done_after_sending_ours;
        nsec::logging::logger _logger;
    };
#endif

    class animation_task
        : public nsec::scheduling::periodic_task<animation_task>
    {
      public:
        explicit animation_task();
        void start()
        {
            nsec::scheduling::periodic_task<animation_task>::start();
        }
        void tick(nsec::scheduling::absolute_time_ms current_time_ms);

      private:
        nsec::logging::logger _logger;
    };

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

    // Handle network events
    enum class badge_discovered_result : uint8_t { NEW, ALREADY_KNOWN };
    badge_discovered_result on_badge_discovered(const uint8_t *id) noexcept;
    void on_badge_discovery_completed() noexcept;

    void _set_network_app_state(network_app_state) noexcept;

    void _set_user_name_scroll_screen() noexcept;

    static uint8_t
    _compute_new_social_level(uint8_t current_social_level,
                              uint16_t new_badges_discovered_count) noexcept;
    void _set_selected_animation(uint8_t animation_id, bool save,
                                 bool set_idle_animation) noexcept;
    // Setup hardware.
    void _setup();

    badge_unique_id _get_unique_id();

    mutable SemaphoreHandle_t _public_access_semaphore;
    uint8_t _social_level = 0;
    uint8_t _selected_animation = 0;
    uint8_t _idle_press_down_tracking = 0;

    network_app_state _current_network_app_state =
        network_app_state::UNCONNECTED;
    unsigned int _badges_discovered_last_exchange = 0;
    bool _is_expecting_factory_reset : 1 = 0;

    button::watcher _button_watcher;

    // network
    communication::network_handler _network_handler;
    //network_id_exchanger _id_exchanger;
    pairing_animator _pairing_animator;
    pairing_completed_animator _pairing_completed_animator;

    nsec::led::strip_animator _strip_animator;

    // animation timer
    animation_task _timer;

    nsec::logging::logger _logger;
};
} // namespace nsec::runtime

#endif // NSEC_RUNTIME_BADGE_HPP
