/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Jérémie Galarneau <jeremie.galarneau@gmail.com>
 * SPDX-FileCopyrightText: 2025 NorthSec
 */

#include "badge-led-strip/strip_animator.hpp"
#include "utils/board.hpp"
#include "utils/round_robin_consumer.hpp"

#include <cmath>
#include <numeric>

namespace nl = nsec::led;
namespace ns = nsec::scheduling;

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(*(array)))

namespace
{
constexpr int16_t scaling_factor = 1024;

// Linear interpolation in RGB space: not terrible, not great...
nl::strip_animator::led_color
interpolate(const nl::strip_animator::keyframe &origin,
            const nl::strip_animator::keyframe &destination,
            uint16_t current_time)
{
    nl::strip_animator::led_color new_color;

    if (current_time >= destination.time) {
        return destination.color;
    }

    const int32_t numerator =
        (int32_t(current_time) - int32_t(origin.time)) * scaling_factor;
    const int32_t denominator =
        (int32_t(destination.time) - int32_t(origin.time)) * scaling_factor;
    for (uint8_t i = 0; i < sizeof(new_color.components); i++) {
        const int32_t origin_component = origin.color.components[i];
        const int32_t destination_component = destination.color.components[i];
        const int32_t total_diff = destination_component - origin_component;
        const int32_t diff_to_apply = ((total_diff * numerator) / denominator);

        new_color.components[i] = uint8_t(origin_component + diff_to_apply);
    }

    return new_color;
}

nl::strip_animator::keyframe
keyframe_from_flash(const nl::strip_animator::keyframe *src_keyframe)
{
    const auto r = src_keyframe->color.r();
    const auto g = src_keyframe->color.g();
    const auto b = src_keyframe->color.b();

    return {{r, g, b}, src_keyframe->time};
}

} // namespace

namespace heartbeat
{

struct template_keyframe {
    uint8_t intensity;
    unsigned int time_ms;
};

const template_keyframe slow_template_keyframes[] = {
    {0, 0}, {255, 1}, {0, 200}, {0, 350}, {255, 350}, {0, 800}, {0, 2000},
};

const nl::strip_animator::led_color colors[] = {
    {255, 0, 0},   {0, 255, 0},   {0, 0, 255},   {255, 255, 255},
    {255, 255, 0}, {0, 255, 255}, {255, 0, 255},
};

// Storage for the current heartbeat keyframes
nl::strip_animator::keyframe
    slow_keyframes[ARRAY_LENGTH(slow_template_keyframes)];

} // namespace heartbeat

namespace keyframes
{
namespace shooting_star
{
// Colors evoking a tungten flash
const nl::strip_animator::keyframe shooting_star_tungsten_keyframes[] = {
    {{0, 0, 0}, 0},         // black
    {{255, 255, 255}, 100}, // white
    {{180, 0, 200}, 400},   // violet
    {{70, 0, 0}, 900},      // red
    {{0, 0, 0}, 1500},      // black
    {{0, 0, 0}, 5000},      // maintain black
};

const nl::strip_animator::keyframe shooting_star_2_keyframes[] = {
    {{0, 255, 159}, 0},
    {{0, 138, 198}, 900},
    {{0, 0, 0}, 1500},
    {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_3_keyframes[] = {
    {{123, 179, 255}, 0}, {{232, 106, 240}, 500}, {{158, 55, 159}, 1000},
    {{0, 0, 0}, 1300},    {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_4_keyframes[] = {
    {{115, 205, 186}, 0}, {{198, 240, 234}, 500}, {{79, 145, 62}, 1000},
    {{0, 0, 0}, 1300},    {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_5_keyframes[] = {
    {{255, 128, 0}, 0},
    {{200, 128, 0}, 600},
    {{0, 0, 0}, 1300},
    {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_6_keyframes[] = {
    {{0, 255, 128}, 0}, {{255, 255, 51}, 500}, {{255, 153, 51}, 1000},
    {{0, 0, 0}, 1300},  {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_7_keyframes[] = {
    {{255, 0, 255}, 0}, {{127, 0, 255}, 500}, {{0, 0, 32}, 1000},
    {{0, 0, 0}, 1300},  {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe shooting_star_8_keyframes[] = {
    {{0, 0, 0}, 0},      {{255, 204, 204}, 100}, {{51, 153, 255}, 400},
    {{76, 0, 153}, 900}, {{0, 0, 0}, 1500},      {{0, 0, 0}, 5000},
};

struct shooting_star_parameters {
    uint8_t shooting_star_count;
    uint16_t delay_advance_ms;
    const nl::strip_animator::keyframe *keyframes;
    uint8_t keyframe_count;
};

const shooting_star_parameters params[] = {
    // white-violet-red, slow
    {1, 90, shooting_star_tungsten_keyframes,
     ARRAY_LENGTH(shooting_star_tungsten_keyframes)},
    {2, 2 * 90, shooting_star_tungsten_keyframes,
     ARRAY_LENGTH(shooting_star_tungsten_keyframes)},
    {4, 3 * 90, shooting_star_tungsten_keyframes,
     ARRAY_LENGTH(shooting_star_tungsten_keyframes)},
    {8, 4 * 90, shooting_star_tungsten_keyframes,
     ARRAY_LENGTH(shooting_star_tungsten_keyframes)},
    // cyan-blue, slow
    {1, 90, shooting_star_2_keyframes, ARRAY_LENGTH(shooting_star_2_keyframes)},
    {2, 2 * 90, shooting_star_2_keyframes,
     ARRAY_LENGTH(shooting_star_2_keyframes)},
    {4, 3 * 90, shooting_star_2_keyframes,
     ARRAY_LENGTH(shooting_star_2_keyframes)},
    {8, 4 * 90, shooting_star_2_keyframes,
     ARRAY_LENGTH(shooting_star_2_keyframes)},
    // violet-blue, fast
    {1, 40, shooting_star_3_keyframes, ARRAY_LENGTH(shooting_star_3_keyframes)},
    {2, 2 * 40, shooting_star_3_keyframes,
     ARRAY_LENGTH(shooting_star_3_keyframes)},
    {4, 3 * 40, shooting_star_3_keyframes,
     ARRAY_LENGTH(shooting_star_3_keyframes)},
    {8, 4 * 40, shooting_star_3_keyframes,
     ARRAY_LENGTH(shooting_star_3_keyframes)},
    // aqua-green , fast
    {1, 40, shooting_star_4_keyframes, ARRAY_LENGTH(shooting_star_4_keyframes)},
    {2, 2 * 40, shooting_star_4_keyframes,
     ARRAY_LENGTH(shooting_star_4_keyframes)},
    {4, 3 * 40, shooting_star_4_keyframes,
     ARRAY_LENGTH(shooting_star_4_keyframes)},
    {8, 4 * 40, shooting_star_4_keyframes,
     ARRAY_LENGTH(shooting_star_4_keyframes)},
    // orange , slow
    {1, 90, shooting_star_5_keyframes, ARRAY_LENGTH(shooting_star_5_keyframes)},
    {2, 2 * 90, shooting_star_5_keyframes,
     ARRAY_LENGTH(shooting_star_5_keyframes)},
    {4, 3 * 90, shooting_star_5_keyframes,
     ARRAY_LENGTH(shooting_star_5_keyframes)},
    {8, 4 * 90, shooting_star_5_keyframes,
     ARRAY_LENGTH(shooting_star_5_keyframes)},
    // green to orange , slow
    {1, 90, shooting_star_6_keyframes, ARRAY_LENGTH(shooting_star_6_keyframes)},
    {2, 2 * 90, shooting_star_6_keyframes,
     ARRAY_LENGTH(shooting_star_6_keyframes)},
    {4, 3 * 90, shooting_star_6_keyframes,
     ARRAY_LENGTH(shooting_star_6_keyframes)},
    {8, 4 * 90, shooting_star_6_keyframes,
     ARRAY_LENGTH(shooting_star_6_keyframes)},
    // violet to blue , slow
    {1, 90, shooting_star_7_keyframes, ARRAY_LENGTH(shooting_star_7_keyframes)},
    {2, 2 * 90, shooting_star_7_keyframes,
     ARRAY_LENGTH(shooting_star_7_keyframes)},
    {4, 3 * 90, shooting_star_7_keyframes,
     ARRAY_LENGTH(shooting_star_7_keyframes)},
    {8, 4 * 90, shooting_star_7_keyframes,
     ARRAY_LENGTH(shooting_star_7_keyframes)},
    // white to blue, slow
    {1, 90, shooting_star_8_keyframes, ARRAY_LENGTH(shooting_star_8_keyframes)},
    {2, 2 * 90, shooting_star_8_keyframes,
     ARRAY_LENGTH(shooting_star_8_keyframes)},
    {4, 3 * 90, shooting_star_8_keyframes,
     ARRAY_LENGTH(shooting_star_8_keyframes)},
    {8, 4 * 90, shooting_star_8_keyframes,
     ARRAY_LENGTH(shooting_star_8_keyframes)},
};

shooting_star_parameters
shooting_star_parameters_from_flash(const shooting_star_parameters *params)
{
    shooting_star_parameters value;

    value.shooting_star_count = params->shooting_star_count;
    value.delay_advance_ms = params->delay_advance_ms;

    value.keyframes = reinterpret_cast<const nl::strip_animator::keyframe *>(
        params->keyframes);
    value.keyframe_count = params->keyframe_count;

    return value;
}
} // namespace shooting_star

namespace color_cycle
{
struct color_cycle_parameters {
    uint16_t active_pattern;
    uint8_t cycle_offset;
    uint8_t keyframe_count;
    const nl::strip_animator::keyframe *keyframes;
};

// Dark wave synth
const nl::strip_animator::keyframe color_cycle_1_keyframes[] = {
    {{0, 0, 0}, 0},         {{255, 73, 219}, 1},    {{28, 28, 62}, 1000},
    {{50, 240, 255}, 2000}, {{255, 129, 50}, 3000}, {{90, 52, 123}, 4000},
    {{255, 73, 219}, 5000},
};

// red-white
const nl::strip_animator::keyframe color_cycle_2_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{255, 0, 0}, 1000},
    {{255, 255, 255}, 2000},
};

// pastel rainbow
const nl::strip_animator::keyframe color_cycle_3_keyframes[] = {
    {{0, 0, 0}, 0},          {{255, 179, 186}, 1},    {{255, 223, 186}, 1000},
    {{255, 255, 186}, 2000}, {{186, 255, 201}, 3000}, {{186, 225, 255}, 4000},
    {{255, 179, 186}, 5000},
};

// orange breathing
const nl::strip_animator::keyframe color_cycle_orange_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{153, 76, 0}, 1},
    {{255, 178, 102}, 2000},
    {{153, 76, 0}, 4000},
};

const nl::strip_animator::keyframe color_cycle_red_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{153, 0, 0}, 1},
    {{255, 102, 102}, 2000},
    {{153, 0, 0}, 4000},
};

const nl::strip_animator::keyframe color_cycle_yellow_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{153, 153, 0}, 1},
    {{255, 255, 102}, 2000},
    {{153, 153, 0}, 4000},
};

const nl::strip_animator::keyframe color_cycle_green_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{0, 153, 0}, 1},
    {{102, 255, 102}, 2000},
    {{0, 153, 0}, 4000},
};

const nl::strip_animator::keyframe color_cycle_cyan_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{0, 153, 153}, 1},
    {{102, 255, 255}, 2000},
    {{0, 153, 153}, 4000},
};

const nl::strip_animator::keyframe color_cycle_blue_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{102, 102, 255}, 1},
    {{0, 0, 153}, 2000},
    {{102, 102, 255}, 4000},
};

const nl::strip_animator::keyframe color_cycle_violet_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{76, 0, 153}, 1},
    {{178, 102, 255}, 2000},
    {{76, 0, 153}, 4000},
};

const nl::strip_animator::keyframe color_cycle_pink_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{153, 0, 153}, 1},
    {{255, 102, 255}, 2000},
    {{153, 0, 153}, 4000},
};

const nl::strip_animator::keyframe color_cycle_white_breathing_keyframes[] = {
    {{0, 0, 0}, 0},
    {{153, 76, 0}, 1},
    {{255, 178, 102}, 2000},
    {{153, 76, 0}, 4000},
};

// Magenta-blue heartbeat
const nl::strip_animator::keyframe color_cycle_magenta_blue_hb_keyframes[] = {
    {{0, 0, 0}, 0},        {{0, 0, 0}, 1},      {{0, 0, 0}, 200},
    {{255, 50, 255}, 300}, {{20, 20, 20}, 400}, {{20, 20, 20}, 500},
    {{25, 25, 127}, 600},  {{0, 0, 0}, 800},    {{0, 0, 0}, 1200},
};

const nl::strip_animator::keyframe color_cycle_green_pink_hb_keyframes[] = {
    {{0, 0, 0}, 0},        {{0, 0, 0}, 1},      {{0, 0, 0}, 200},
    {{50, 200, 50}, 300},  {{10, 10, 10}, 400}, {{10, 10, 10}, 500},
    {{200, 50, 170}, 600}, {{0, 0, 0}, 800},    {{0, 0, 0}, 1200},
};

// purple-white
const nl::strip_animator::keyframe color_cycle_4_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{101, 78, 146}, 1000},
    {{255, 255, 255}, 2000},
};

// yellow-white
const nl::strip_animator::keyframe color_cycle_5_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{249, 245, 75}, 1000},
    {{255, 255, 255}, 2000},
};

// cyan-white
const nl::strip_animator::keyframe color_cycle_6_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{139, 245, 250}, 1000},
    {{255, 255, 255}, 2000},
};

// orange-white
const nl::strip_animator::keyframe color_cycle_7_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{252, 115, 0}, 1000},
    {{255, 255, 255}, 2000},
};

// lime-white
const nl::strip_animator::keyframe color_cycle_8_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{130, 205, 71}, 1000},
    {{255, 255, 255}, 2000},
};

// pink-white
const nl::strip_animator::keyframe color_cycle_9_keyframes[] = {
    {{0, 0, 0}, 0},
    {{255, 255, 255}, 1},
    {{249, 0, 191}, 1000},
    {{255, 255, 255}, 2000},
};

const nl::strip_animator::keyframe color_cycle_10_keyframes[] = {
    {{0, 0, 0}, 0},      {{242, 227, 219}, 100}, {{65, 100, 74}, 200},
    {{38, 58, 41}, 300}, {{232, 106, 51}, 400},  {{242, 227, 219}, 500},
};

const nl::strip_animator::keyframe color_cycle_11_keyframes[] = {
    {{0, 0, 0}, 0},        {{255, 184, 76}, 100}, {{242, 102, 171}, 200},
    {{164, 89, 209}, 300}, {{44, 211, 225}, 400}, {{255, 184, 76}, 500},
};

// Pride
const nl::strip_animator::keyframe color_cycle_12_keyframes[] = {
    {{0, 0, 0}, 0},         {{228, 3, 3}, 500},    {{228, 3, 3}, 950},
    {{255, 140, 0}, 1000},  {{255, 140, 0}, 1450}, {{255, 237, 0}, 1500},
    {{255, 237, 0}, 1950},  {{0, 128, 38}, 2000},  {{0, 128, 38}, 2450},
    {{36, 64, 142}, 2500},  {{36, 64, 142}, 2950}, {{115, 41, 130}, 3000},
    {{115, 41, 130}, 3450}, {{228, 3, 3}, 3500},
};

const nl::strip_animator::keyframe color_cycle_13_keyframes[] = {
    {{0, 0, 0}, 0},         {{0, 0, 0}, 1},         {{21, 4, 133}, 500},
    {{21, 4, 133}, 1000},   {{0, 0, 0}, 1100},      {{0, 0, 0}, 1250},
    {{198, 42, 136}, 1750}, {{198, 42, 136}, 2250}, {{0, 0, 0}, 2350},
    {{0, 0, 0}, 2450},      {{3, 196, 161}, 2950},  {{3, 196, 161}, 3450},
    {{0, 0, 0}, 3550},      {{0, 0, 0}, 3650},
};

const nl::strip_animator::keyframe color_cycle_14_keyframes[] = {
    {{0, 0, 0}, 0},         {{0, 0, 0}, 1},         {{31, 138, 112}, 500},
    {{31, 138, 112}, 1000}, {{0, 0, 0}, 1100},      {{0, 0, 0}, 1250},
    {{191, 219, 56}, 1750}, {{191, 219, 56}, 2250}, {{0, 0, 0}, 2350},
    {{0, 0, 0}, 2450},      {{252, 115, 0}, 2950},  {{252, 115, 0}, 3450},
    {{0, 0, 0}, 3550},      {{0, 0, 0}, 3650},
};

const nl::strip_animator::keyframe color_cycle_spark_1_keyframes[] = {
    {{0, 0, 0}, 0},    {{255, 255, 255}, 100}, {{180, 0, 200}, 400},
    {{70, 0, 0}, 900}, {{0, 0, 0}, 1500},      {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe color_cycle_spark_2_keyframes[] = {
    {{0, 0, 0}, 0},      {{230, 238, 117}, 100}, {{218, 73, 73}, 400},
    {{79, 28, 76}, 900}, {{0, 0, 0}, 1500},      {{0, 0, 0}, 5000},
};

const nl::strip_animator::keyframe color_cycle_spark_3_keyframes[] = {
    {{0, 0, 0}, 0},      {{160, 193, 184}, 100}, {{113, 159, 176}, 400},
    {{53, 31, 57}, 900}, {{0, 0, 0}, 1500},      {{0, 0, 0}, 5000},
};

const color_cycle_parameters params[] = {
    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_13_keyframes),
     color_cycle_13_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_13_keyframes),
     color_cycle_13_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_13_keyframes),
     color_cycle_13_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_14_keyframes),
     color_cycle_14_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_14_keyframes),
     color_cycle_14_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_14_keyframes),
     color_cycle_14_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_10_keyframes),
     color_cycle_10_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_10_keyframes),
     color_cycle_10_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_10_keyframes),
     color_cycle_10_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_11_keyframes),
     color_cycle_11_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_11_keyframes),
     color_cycle_11_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_11_keyframes),
     color_cycle_11_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_12_keyframes),
     color_cycle_12_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_12_keyframes),
     color_cycle_12_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_12_keyframes),
     color_cycle_12_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_1_keyframes), color_cycle_1_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_1_keyframes),
     color_cycle_1_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_1_keyframes),
     color_cycle_1_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_2_keyframes), color_cycle_2_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_2_keyframes),
     color_cycle_2_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_2_keyframes),
     color_cycle_2_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_3_keyframes), color_cycle_3_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_3_keyframes),
     color_cycle_3_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_3_keyframes),
     color_cycle_3_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_4_keyframes), color_cycle_4_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_4_keyframes),
     color_cycle_4_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_4_keyframes),
     color_cycle_4_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_5_keyframes), color_cycle_5_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_5_keyframes),
     color_cycle_5_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_5_keyframes),
     color_cycle_5_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_6_keyframes), color_cycle_6_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_6_keyframes),
     color_cycle_6_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_6_keyframes),
     color_cycle_6_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_7_keyframes), color_cycle_7_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_7_keyframes),
     color_cycle_7_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_7_keyframes),
     color_cycle_7_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_8_keyframes), color_cycle_8_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_8_keyframes),
     color_cycle_8_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_8_keyframes),
     color_cycle_8_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_spark_1_keyframes),
     color_cycle_spark_1_keyframes},
    {0xFFFF, 70, ARRAY_LENGTH(color_cycle_spark_1_keyframes),
     color_cycle_spark_1_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_spark_2_keyframes),
     color_cycle_spark_2_keyframes},
    {0xFFFF, 70, ARRAY_LENGTH(color_cycle_spark_2_keyframes),
     color_cycle_spark_2_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_spark_3_keyframes),
     color_cycle_spark_3_keyframes},
    {0xFFFF, 70, ARRAY_LENGTH(color_cycle_spark_3_keyframes),
     color_cycle_spark_3_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_9_keyframes), color_cycle_9_keyframes},
    {0xFFFF, 10, ARRAY_LENGTH(color_cycle_9_keyframes),
     color_cycle_9_keyframes},
    {0b1010101010101010, 10, ARRAY_LENGTH(color_cycle_9_keyframes),
     color_cycle_9_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_orange_breathing_keyframes),
     color_cycle_orange_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_orange_breathing_keyframes),
     color_cycle_orange_breathing_keyframes},
    {0b1010101010101010, 60,
     ARRAY_LENGTH(color_cycle_orange_breathing_keyframes),
     color_cycle_orange_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_red_breathing_keyframes),
     color_cycle_red_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_red_breathing_keyframes),
     color_cycle_red_breathing_keyframes},
    {0b1010101010101010, 60, ARRAY_LENGTH(color_cycle_red_breathing_keyframes),
     color_cycle_red_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_yellow_breathing_keyframes),
     color_cycle_yellow_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_yellow_breathing_keyframes),
     color_cycle_yellow_breathing_keyframes},
    {0b1010101010101010, 60,
     ARRAY_LENGTH(color_cycle_yellow_breathing_keyframes),
     color_cycle_yellow_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_green_breathing_keyframes),
     color_cycle_green_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_green_breathing_keyframes),
     color_cycle_green_breathing_keyframes},
    {0b1010101010101010, 60,
     ARRAY_LENGTH(color_cycle_green_breathing_keyframes),
     color_cycle_green_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_cyan_breathing_keyframes),
     color_cycle_cyan_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_cyan_breathing_keyframes),
     color_cycle_cyan_breathing_keyframes},
    {0b1010101010101010, 60, ARRAY_LENGTH(color_cycle_cyan_breathing_keyframes),
     color_cycle_cyan_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_blue_breathing_keyframes),
     color_cycle_blue_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_blue_breathing_keyframes),
     color_cycle_blue_breathing_keyframes},
    {0b1010101010101010, 60, ARRAY_LENGTH(color_cycle_blue_breathing_keyframes),
     color_cycle_blue_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_violet_breathing_keyframes),
     color_cycle_violet_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_violet_breathing_keyframes),
     color_cycle_violet_breathing_keyframes},
    {0b1010101010101010, 60,
     ARRAY_LENGTH(color_cycle_violet_breathing_keyframes),
     color_cycle_violet_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_pink_breathing_keyframes),
     color_cycle_pink_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_pink_breathing_keyframes),
     color_cycle_pink_breathing_keyframes},
    {0b1010101010101010, 60, ARRAY_LENGTH(color_cycle_pink_breathing_keyframes),
     color_cycle_pink_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_white_breathing_keyframes),
     color_cycle_white_breathing_keyframes},
    {0xFFFF, 60, ARRAY_LENGTH(color_cycle_white_breathing_keyframes),
     color_cycle_white_breathing_keyframes},
    {0b1010101010101010, 60,
     ARRAY_LENGTH(color_cycle_white_breathing_keyframes),
     color_cycle_white_breathing_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_magenta_blue_hb_keyframes),
     color_cycle_magenta_blue_hb_keyframes},
    {0b1010101010101010, 0, ARRAY_LENGTH(color_cycle_magenta_blue_hb_keyframes),
     color_cycle_magenta_blue_hb_keyframes},

    {0xFFFF, 0, ARRAY_LENGTH(color_cycle_green_pink_hb_keyframes),
     color_cycle_green_pink_hb_keyframes},
    {0b1010101010101010, 0, ARRAY_LENGTH(color_cycle_green_pink_hb_keyframes),
     color_cycle_green_pink_hb_keyframes},
};

color_cycle_parameters
color_cycle_parameters_from_flash(const color_cycle_parameters *params)
{
    color_cycle_parameters value;

    value.active_pattern = params->active_pattern;
    value.cycle_offset = params->cycle_offset;

    value.keyframes = reinterpret_cast<const nl::strip_animator::keyframe *>(
        params->keyframes);
    value.keyframe_count = params->keyframe_count;

    return value;
}

} // namespace color_cycle

} // namespace keyframes

nl::strip_animator::strip_animator() noexcept
    : ns::periodic_task<strip_animator>(
          100) /* Set by the various animations. */,
      _pixels(nsec::board::neopixel::count, nsec::board::neopixel::ctrl_pin, 0),
      _logger("Strip animator", nsec::config::logging::animator_level)
{
    _setup();
    set_idle_animation(0);
}

void nl::strip_animator::_setup() noexcept
{
}

uint8_t
nl::strip_animator::_get_keyframe_index(const indice_storage_element *indices,
                                        uint8_t led_id) const noexcept
{
    const auto &element = indices[led_id / 2];

    return led_id & 1 ? element.odd : element.even;
}

void nl::strip_animator::_set_keyframe_index(
    indice_storage_element *indices, uint8_t led_id,
    uint8_t new_keyframe_index) noexcept
{
    auto &element = indices[led_id / 2];

    if (led_id & 1) {
        element.odd = new_keyframe_index;
    } else {
        element.even = new_keyframe_index;
    }
}

void nl::strip_animator::_keyframe_animation_tick(
    const scheduling::absolute_time_ms &current_time_ms) noexcept
{
    // Animation specific setup
    switch (_config.keyframed._animation) {
    case keyframed_animation::SHOOTING_STAR:
        if (_state.keyframed.shooting_star.ticks_in_position ==
            _config.keyframed.shooting_star.ticks_before_advance) {
            _state.keyframed.shooting_star.ticks_in_position = 0;
            // Stored on 4 bits, wraps around at 15.
            _state.keyframed.shooting_star.position++;

            const auto led_interval =
                16 / _config.keyframed.shooting_star.star_count;
            for (uint8_t i = 0; i < _config.keyframed.shooting_star.star_count;
                 i++) {
                const auto position = (_state.keyframed.shooting_star.position +
                                       (led_interval * i)) %
                                      16;

                _set_keyframe_index(_state.keyframed.origin_keyframe_index,
                                    position, 0);
                _set_keyframe_index(_state.keyframed.destination_keyframe_index,
                                    position, 0);

                _state.keyframed.ticks_since_start_of_animation[position] = 0;
                _config.keyframed.active |= 1 << position;
            }
        }

        _state.keyframed.shooting_star.ticks_in_position++;
        break;
    default:
        break;
    }

    for (unsigned int i = 0; i < nsec::board::neopixel::anim_count; i++) {
        const bool led_animation_is_active =
            (_config.keyframed.active >> i) & 1;

        if (!led_animation_is_active) {
            // Inactive, repeat the origin keyframe.
            _set_keyframe_index(_state.keyframed.origin_keyframe_index, i,
                                0);
            _set_keyframe_index(_state.keyframed.destination_keyframe_index,
                                i, 0);

            const auto origin_keyframe_index =
                _get_keyframe_index(_state.keyframed.origin_keyframe_index, i);
            const auto origin_keyframe = keyframe_from_flash(
                &_config.keyframed.keyframes[origin_keyframe_index]);

            _pixels.setPixelColor(i, origin_keyframe.color.r(),
                                  origin_keyframe.color.g(),
                                  origin_keyframe.color.b());
            continue;
        }

        const auto origin_keyframe_index =
            _get_keyframe_index(_state.keyframed.origin_keyframe_index, i);
        const auto destination_keyframe_index =
            _get_keyframe_index(_state.keyframed.destination_keyframe_index, i);
        const auto origin_keyframe = keyframe_from_flash(
            &_config.keyframed.keyframes[origin_keyframe_index]);

        const auto time_since_animation_start =
            _state.keyframed.ticks_since_start_of_animation[i] * period_ms();

        if (_state.keyframed.ticks_since_start_of_animation[i] !=
            std::numeric_limits<std::decay_t<
                decltype(*_state.keyframed.ticks_since_start_of_animation)>>::
                max()) {
            // Saturate counter.
            _state.keyframed.ticks_since_start_of_animation[i]++;
        }

        // Interpolate to find the current color.
        const auto destination_keyframe = keyframe_from_flash(
            &_config.keyframed.keyframes[destination_keyframe_index]);

        // destination_keyframe.color.log(F("destination keyframe: "));

        const auto new_color =
            interpolate(origin_keyframe, destination_keyframe,
                        uint16_t(time_since_animation_start));
        // new_color.log(F("interpolated color: "));

        _pixels.setPixelColor(i, new_color.r(), new_color.g(), new_color.b());

        // Advance keyframes if needed.
        if (time_since_animation_start < destination_keyframe.time) {
            continue;
        }

        uint8_t new_origin_keyframe_index = destination_keyframe_index;
        uint8_t new_destination_keyframe_index = destination_keyframe_index + 1;
        if (new_destination_keyframe_index >=
            _config.keyframed.keyframe_count) {
            // Loop back to the configured loop point. Move the current
            // animation time in the past.
            new_origin_keyframe_index = _config.keyframed.loop_point_index;
            new_destination_keyframe_index =
                std::min(new_origin_keyframe_index + 1,
                         _config.keyframed.keyframe_count - 1);
            // Round up to make sure the time isn't _before_ the origin
            // keyframe.
            _state.keyframed.ticks_since_start_of_animation[i] =
                ((keyframe_from_flash(
                      &_config.keyframed.keyframes[new_origin_keyframe_index])
                      .time +
                  (period_ms() - 1)) /
                 period_ms());
        } else {
            new_origin_keyframe_index = destination_keyframe_index;
            new_destination_keyframe_index = destination_keyframe_index + 1;
        }

        _set_keyframe_index(_state.keyframed.origin_keyframe_index, i,
                            new_origin_keyframe_index);
        _set_keyframe_index(_state.keyframed.destination_keyframe_index, i,
                            new_destination_keyframe_index);
    }

    _pixels.setBrightness(_config.keyframed.brightness);
}

void nl::strip_animator::tick(
    scheduling::absolute_time_ms current_time_ms) noexcept
{
    switch (_current_animation_type) {
    case animation_type::KEYFRAMED:
        _keyframe_animation_tick(current_time_ms);
        break;
    default:
        break;
    }

    // Send the updated pixel colors to the hardware.
    _pixels.show();
}

nl::strip_animator::led_color
nl::strip_animator::_color(uint8_t led_id) const noexcept
{
    return _pixels.get_color(led_id);
}

void nl::strip_animator::set_idle_animation(uint8_t id) noexcept
{
    _logger.info("Idle animation set: id={}", id);

    const auto shooting_star_animations_count =
        ARRAY_LENGTH(keyframes::shooting_star::params);
    const auto color_cycle_animations_count =
        ARRAY_LENGTH(keyframes::color_cycle::params);
    const auto slow_heartbeat_animations_count =
        ARRAY_LENGTH(heartbeat::colors);

    constexpr std::array<std::size_t, 3> animation_counts = {
        slow_heartbeat_animations_count, color_cycle_animations_count,
        shooting_star_animations_count};
    nsec::utils::array_round_robin_consumer<animation_counts.size()> consumer(
        animation_counts);

    // Limit 'id' to the range of existing animations
    id = id %
         std::accumulate(animation_counts.begin(), animation_counts.end(), 0);

    nsec::utils::array_round_robin_consumer<
        animation_counts.size()>::element_description chosen_animation;

    for (auto i = 0; i <= id; i++) {
        chosen_animation = consumer.consume();
    }

    _logger.info("Selected animation: animation_type={}, animation_id={}",
                 chosen_animation.array_id, chosen_animation.index);

    switch (chosen_animation.array_id) {
    case 0: {
        const auto &color = heartbeat::colors[chosen_animation.index];

        _logger.debug("Heartbeat color set: ({}, {}, {})", color.r(), color.g(),
                      color.b());

        // Build keyframes for the heartbeat animation
        for (std::size_t keyframe_number = 0;
             keyframe_number < ARRAY_LENGTH(heartbeat::slow_template_keyframes);
             keyframe_number++) {
            const auto &keyframe_template =
                heartbeat::slow_template_keyframes[keyframe_number];

            auto &keyframe = heartbeat::slow_keyframes[keyframe_number];
            keyframe.time = keyframe_template.time_ms;

            const auto intensity = keyframe_template.intensity;
            for (unsigned int component_id = 0;
                 component_id < ARRAY_LENGTH(color.components);
                 component_id++) {
                const auto blended_component =
                    (float(intensity) / 255.0f) *
                    float(color.components[component_id]);
                keyframe.color.components[component_id] =
                    std::uint8_t(std::lround(blended_component));
            }

            _logger.debug("Initialized keyframe: number={}, color=({}, {}, "
                          "{}), intensity={}",
                          keyframe_number, keyframe.color.r(),
                          keyframe.color.g(), keyframe.color.b(), intensity);
        }

        _set_keyframed_cycle_animation(heartbeat::slow_keyframes,
                                       ARRAY_LENGTH(heartbeat::slow_keyframes),
                                       1, 0xFFFF, 0, 10);
        _config.keyframed._animation = keyframed_animation::HEART_BEAT;
        break;
    }
    case 1: {
        const auto cc_params =
            keyframes::color_cycle::color_cycle_parameters_from_flash(
                &keyframes::color_cycle::params[chosen_animation.index]);

        _set_keyframed_cycle_animation(
            cc_params.keyframes, cc_params.keyframe_count, 1,
            cc_params.active_pattern, cc_params.cycle_offset, 10);
        break;
    }
    case 2: {
        const auto ss_params =
            keyframes::shooting_star::shooting_star_parameters_from_flash(
                &keyframes::shooting_star::params[chosen_animation.index]);

        _set_shooting_star_animation(
            ss_params.shooting_star_count, ss_params.delay_advance_ms,
            ss_params.keyframes, ss_params.keyframe_count);
        break;
    }
    }
}

void nl::strip_animator::_reset_keyframed_animation_state() noexcept
{
    memset(&_state.keyframed, 0, sizeof(_state.keyframed));
}


void nl::strip_animator::set_clearance_meter_bar(uint8_t led_count)
{
    if ((nsec::board::neopixel::anim_count + led_count) > nsec::board::neopixel::count) {
        NSEC_THROW_ERROR(
            fmt::format("Invalid LED count provided to set_clearance_meter_bar: "
                        "led_count={}, max_allowed_count={}",
                        led_count, nsec::board::neopixel::count));
    }

    _logger.info("Setting clearance meter bar animation: led_count={}", led_count);

    for (uint8_t i = 0; i < led_count; i++) {
            _pixels.setPixelColor(nsec::board::neopixel::anim_count + i, 0x00, 0xFF, 0x00);
            _pixels.setPixelBrightness(nsec::board::neopixel::anim_count + i, 150);
    }
}

void nl::strip_animator::set_blank_animation()
{
    _reset_keyframed_animation_state();
    _config.keyframed.active = 0;

    tick(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

void nl::strip_animator::_set_shooting_star_animation(
    uint8_t star_count, unsigned int advance_interval_ms,
    const keyframe *keyframes, uint8_t keyframe_count) noexcept
{
    period_ms(10);
    _current_animation_type = animation_type::KEYFRAMED;
    _config.keyframed._animation = keyframed_animation::SHOOTING_STAR;
    _config.keyframed.active = 0;
    _reset_keyframed_animation_state();
    _config.keyframed.keyframe_count = keyframe_count;
    _config.keyframed.keyframes = keyframes;

    _config.keyframed.loop_point_index = 0;
    _config.keyframed.brightness = 50;
    _config.keyframed.shooting_star.ticks_before_advance =
        advance_interval_ms / period_ms();
    _config.keyframed.shooting_star.star_count = star_count;
}

void nl::strip_animator::_set_keyframed_cycle_animation(
    const keyframe *keyframe, uint8_t keyframe_count, uint8_t loop_point_index,
    uint16_t active_mask, uint8_t cycle_offset_between_frames,
    uint8_t refresh_rate) noexcept
{
    period_ms(refresh_rate);

    // Setup animation parameters.
    _current_animation_type = animation_type::KEYFRAMED;
    _config.keyframed._animation = keyframed_animation::CYCLE;
    _config.keyframed.active = active_mask;

    _reset_keyframed_animation_state();

    _config.keyframed.keyframe_count = keyframe_count;
    _config.keyframed.keyframes = keyframe;

    // Apply an offset between LEDs to achieve a "sparkle" effect.
    for (uint8_t i = 0; i < 16; i++) {
        _state.keyframed.ticks_since_start_of_animation[i] =
            (i * cycle_offset_between_frames) %
            (keyframe_from_flash(
                 &_config.keyframed
                      .keyframes[_config.keyframed.keyframe_count - 1])
                 .time /
             period_ms());
    }

    _config.keyframed.loop_point_index = loop_point_index;
    _config.keyframed.brightness = 50;
}
