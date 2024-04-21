/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 */

#ifndef NSEC_LED_STRIP_ANIMATOR_HPP
#define NSEC_LED_STRIP_ANIMATOR_HPP

#include "scheduling/task.hpp"
#include "utils/board.hpp"
#include "utils/logging.hpp"

#include <SmartLeds.h>

#include <cstring>

namespace nsec::led
{

class strip_animator : public scheduling::periodic_task<strip_animator>
{
    friend class periodic_task<strip_animator>;

  public:
    strip_animator() noexcept;

    /* Deactivate copy and assignment. */
    strip_animator(const strip_animator &) = delete;
    strip_animator(strip_animator &&) = delete;
    strip_animator &operator=(const strip_animator &) = delete;
    strip_animator &operator=(strip_animator &&) = delete;
    ~strip_animator() = default;

    void set_idle_animation(uint8_t id) noexcept;

    void set_red_to_green_led_progress_bar(uint8_t led_count) noexcept;
    void set_health_meter_bar(uint8_t led_count) noexcept;

    enum class pairing_completed_animation_type : uint8_t {
        HAPPY_CLOWN_BARF,
        NO_NEW_FRIENDS,
        IDLE_SOCIAL_LEVEL
    };
    void set_pairing_completed_animation(
        pairing_completed_animation_type) noexcept;
    void set_show_level_animation(pairing_completed_animation_type,
                                  uint8_t level,
                                  bool set_lower_bar_on) noexcept;

    struct led_color {
        led_color() = default;
        constexpr led_color(uint8_t r_in, uint8_t g_in, uint8_t b_in)
        {
            components[np_green_offset] = g_in;
            components[np_red_offset] = r_in;
            components[np_blue_offset] = b_in;
        }

        explicit led_color(uint8_t *native_device_pixel_storage)
        {
            std::memcpy(components, native_device_pixel_storage,
                        sizeof(components));
        }

        constexpr led_color(const led_color &in_color)
        {
            components[np_green_offset] = in_color.g();
            components[np_red_offset] = in_color.r();
            components[np_blue_offset] = in_color.b();
        }

        constexpr const uint8_t &r() const noexcept
        {
            return components[np_red_offset];
        }

        constexpr const uint8_t &g() const noexcept
        {
            return components[np_green_offset];
        }

        constexpr const uint8_t &b() const noexcept
        {
            return components[np_blue_offset];
        }

        uint8_t components[3];

      private:
        // Offsets in the three-byte group forming a pixel given our device's
        // internal pixel format.
        static const std::uint8_t np_red_offset = 0;
        static const std::uint8_t np_green_offset = 1;
        static const std::uint8_t np_blue_offset = 2;
    };

    struct keyframe {
        keyframe() = default;
        constexpr keyframe(const led_color &in_color, unsigned int at_time)
            : color{in_color}, time{at_time}
        {
        }

        led_color color;
        // Time (in ms) since the animation started.
        unsigned int time;
    };

  protected:
    void tick(scheduling::absolute_time_ms current_time_ms) noexcept;

  private:
    enum class animation_type : uint8_t {
        LEGACY,
        KEYFRAMED,
    };

    enum class keyframed_animation : uint8_t {
        PROGRESS_BAR,
        SHOOTING_STAR, // Shooting star running accross the LEDs
        SPARKS,        // Random sparks appearing
        CYCLE,
    };

    void _setup() noexcept;
    void _keyframe_animation_tick(
        const scheduling::absolute_time_ms &current_time_ms) noexcept;
    led_color _color(uint8_t led_id) const noexcept;
    void _reset_keyframed_animation_state() noexcept;

    class NeoStub
    {
      public:
        NeoStub(unsigned int count, unsigned ctrl_pin, int flags)
            : _leds(LED_WS2812B, nsec::board::neopixel::count,
                    nsec::board::neopixel::ctrl_pin, 0, DoubleBuffer),
              _logger("NeoStub"){};
        ~NeoStub() = default;

        void setBrightness(std::uint8_t brightness)
        {
            for (auto &led : _leds) {
                led.stretchChannels(brightness, brightness, brightness);
            }
        }

        void setPixelColor(std::uint8_t pixel_id, std::uint8_t r,
                           std::uint8_t g, std::uint8_t b)
        {
            _leds[pixel_id] = Rgb(r, g, b);
        }

        void show()
        {
            const auto ret = _leds.show();
            if (ret != ESP_OK) {
                _logger.error("Failed to show LEDs");
            }
        }

        led_color get_color(unsigned int index) const
        {
            const auto &color = _leds[index];

            return led_color(color.r, color.g, color.b);
        }

      private:
        std::uint8_t _pixies[32 * 8];
        SmartLed _leds;
        nsec::logging::logger _logger;
    };

    NeoStub _pixels;
    animation_type _current_animation_type;

    // Keyframe indices of 4-bits each, use helpers to access.
    struct indice_storage_element {
        uint8_t even : 4;
        uint8_t odd : 4;
    };

    union {
        struct {
            uint8_t level;
        } legacy;
        struct {
            union {
                struct {
                    uint8_t ticks_before_advance;
                    uint8_t star_count;
                } shooting_star;
            };
            uint8_t keyframe_count : 4;
            uint8_t loop_point_index : 4;
            keyframed_animation _animation;
            const keyframe *keyframes;
            // 1-bit per led. When inactive, the origin keyframe is repeated.
            uint16_t active;
            uint8_t brightness;
        } keyframed;
    } _config;
    union {
        struct {
            uint8_t start_position;
        } legacy;
        struct {
            union {
                struct {
                    uint8_t position : 4;
                    uint8_t ticks_in_position;
                } shooting_star;
            };

            indice_storage_element origin_keyframe_index[8];
            indice_storage_element destination_keyframe_index[8];
            uint8_t ticks_since_start_of_animation[16];
        } keyframed;
    } _state;
    nsec::logging::logger _logger;

    uint8_t _get_keyframe_index(const indice_storage_element *indices,
                                uint8_t led_id) const noexcept;
    void _set_keyframe_index(indice_storage_element *indices, uint8_t led_id,
                             uint8_t index) noexcept;

    void _set_shooting_star_animation(uint8_t star_count,
                                      unsigned int advance_interval_ms,
                                      const keyframe *keyframe,
                                      uint8_t keyframe_count) noexcept;

    void _set_keyframed_cycle_animation(const keyframe *keyframe,
                                        uint8_t keyframe_count,
                                        uint8_t loop_point_index,
                                        uint16_t active_mask,
                                        uint8_t cycle_offset_between_frames,
                                        uint8_t refresh_rate) noexcept;
};
} // namespace nsec::led
#endif // NSEC_LED_STRIP_ANIMATOR_HPP
