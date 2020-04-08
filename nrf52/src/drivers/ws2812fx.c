/*
  WS2812FX.cpp - Library for WS2812 LED effects.

  Harm Aldick - 2016
  www.aldick.org


  FEATURES
    * A lot of blinken modes and counting
    * WS2812FX can be used as drop-in replacement for Adafruit NeoPixel Library

  NOTES
    * Uses the Adafruit NeoPixel library. Get it here:
      https://github.com/adafruit/Adafruit_NeoPixel



  LICENSE

  The MIT License (MIT)

  Copyright (c) 2016  Harm Aldick

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.


  CHANGELOG

  2016-05-28   Initial beta release
  2016-06-03   Code cleanup, minor improvements, new modes
  2016-06-04   2 new fx, fixed setColor (now also resets _mode_color)
  2017-02-02   added external trigger functionality (e.g. for sound-to-light)
  2017-02-02   removed "blackout" on mode, speed or color-change
  2017-09-26   implemented segment and reverse features
  2017-11-16   changed speed calc, reduced memory footprint
  2018-02-24   added hooks for user created custom effects
*/

#include "ws2812fx.h"
#include "app/timer.h"
#include "app/utils.h"
#include "led_effects.h"
#include <arm_math.h>
#include <nrf.h>
#include <nrf_delay.h>
#include <stdlib.h>
#include <string.h>

uint8_t get_random_wheel_index(uint8_t);
void strip_off_WS2812FX(void);

void strip_off(void);
void fade_out(void);

uint16_t mode_static(void);
uint16_t blink(uint32_t, uint32_t, bool strobe);
uint16_t mode_blink(void);
uint16_t mode_blink_rainbow(void);
uint16_t mode_strobe(void);
uint16_t mode_strobe_rainbow(void);
uint16_t color_wipe(uint32_t, uint32_t, bool);
uint16_t mode_color_wipe(void);
uint16_t mode_color_wipe_inv(void);
uint16_t mode_color_wipe_rev(void);
uint16_t mode_color_wipe_rev_inv(void);
uint16_t mode_color_wipe_random(void);
uint16_t mode_color_sweep_random(void);
uint16_t mode_random_color(void);
uint16_t mode_single_dynamic(void);
uint16_t mode_multi_dynamic(void);
uint16_t mode_breath(void);
uint16_t mode_fade(void);
uint16_t mode_scan(void);
uint16_t mode_dual_scan(void);
uint16_t theater_chase(uint32_t, uint32_t);
uint16_t mode_theater_chase(void);
uint16_t mode_theater_chase_rainbow(void);
uint16_t mode_rainbow(void);
uint16_t mode_rainbow_cycle(void);
uint16_t mode_running_lights(void);
uint16_t twinkle(uint32_t);
uint16_t mode_twinkle(void);
uint16_t mode_twinkle_random(void);
uint16_t twinkle_fade(uint32_t);
uint16_t mode_twinkle_fade(void);
uint16_t mode_twinkle_fade_random(void);
uint16_t mode_sparkle(void);
uint16_t mode_flash_sparkle(void);
uint16_t mode_hyper_sparkle(void);
uint16_t mode_multi_strobe(void);
uint16_t chase(uint32_t, uint32_t, uint32_t);
uint16_t mode_chase_white(void);
uint16_t mode_chase_color(void);
uint16_t mode_chase_random(void);
uint16_t mode_chase_rainbow(void);
uint16_t mode_chase_flash(void);
uint16_t mode_chase_flash_random(void);
uint16_t mode_chase_rainbow_white(void);
uint16_t mode_chase_blackout(void);
uint16_t mode_chase_blackout_rainbow(void);
uint16_t running(uint32_t, uint32_t);
uint16_t mode_running_color(void);
uint16_t mode_running_red_blue(void);
uint16_t mode_running_random(void);
uint16_t mode_larson_scanner(void);
uint16_t mode_comet(void);
uint16_t fireworks(uint32_t);
uint16_t mode_fireworks(void);
uint16_t mode_fireworks_random(void);
uint16_t mode_merry_christmas(void);
uint16_t mode_halloween(void);
uint16_t mode_fire_flicker(void);
uint16_t mode_fire_flicker_soft(void);
uint16_t mode_fire_flicker_intense(void);
uint16_t fire_flicker(int);
uint16_t mode_circus_combustus(void);
uint16_t tricolor_chase(uint32_t, uint32_t, uint32_t);
uint16_t mode_bicolor_chase(void);
uint16_t mode_tricolor_chase(void);
uint16_t mode_icu(void);
uint16_t mode_custom(void);

uint16_t (*mode[])(void) = {
    mode_static,
    mode_blink,
    mode_breath,
    mode_color_wipe,
    mode_color_wipe_inv,
    mode_color_wipe_rev,
    mode_color_wipe_rev_inv,
    mode_color_wipe_random,
    mode_random_color,
    mode_single_dynamic,
    mode_multi_dynamic,
    mode_rainbow,
    mode_rainbow_cycle,
    mode_scan,
    mode_dual_scan,
    mode_fade,
    mode_theater_chase,
    mode_theater_chase_rainbow,
    mode_running_lights,
    mode_twinkle,
    mode_twinkle_random,
    mode_twinkle_fade,
    mode_twinkle_fade_random,
    mode_sparkle,
    mode_flash_sparkle,
    mode_hyper_sparkle,
    mode_strobe,
    mode_strobe_rainbow,
    mode_multi_strobe,
    mode_blink_rainbow,
    mode_chase_white,
    mode_chase_color,
    mode_chase_random,
    mode_chase_rainbow,
    mode_chase_flash,
    mode_chase_flash_random,
    mode_chase_rainbow_white,
    mode_chase_blackout,
    mode_chase_blackout_rainbow,
    mode_color_sweep_random,
    mode_running_color,
    mode_running_red_blue,
    mode_running_random,
    mode_larson_scanner,
    mode_comet,
    mode_fireworks,
    mode_fireworks_random,
    mode_merry_christmas,
    mode_fire_flicker,
    mode_fire_flicker_soft,
    mode_fire_flicker_intense,
    mode_circus_combustus,
    mode_halloween,
    mode_bicolor_chase,
    mode_tricolor_chase,
    mode_icu,
    mode_custom,
};

const char *name[] = {
    "Static",
    "Blink",
    "Breath",
    "Color Wipe",
    "Color Wipe Inverse",
    "Color Wipe Reverse",
    "Color Wipe Reverse Inverse",
    "Color Wipe Random",
    "Random Color",
    "Single Dynamic",
    "Multi Dynamic",
    "Rainbow",
    "Rainbow Cycle",
    "Scan",
    "Dual Scan",
    "Fade",
    "Theater Chase",
    "Theater Chase Rainbow",
    "Running Lights",
    "Twinkle",
    "Twinkle Random",
    "Twinkle Fade",
    "Twinkle Fade Random",
    "Sparkle",
    "Flash Sparkle",
    "Hyper Sparkle",
    "Strobe",
    "Strobe Rainbow",
    "Multi Strobe",
    "Blink Rainbow",
    "Chase White",
    "Chase Color",
    "Chase Random",
    "Chase Rainbow",
    "Chase Flash",
    "Chase Flash Random",
    "Chase Rainbow White",
    "Chase Blackout",
    "Chase Blackout Rainbow",
    "Color Sweep Random",
    "Running Color",
    "Running Red Blue",
    "Running Random",
    "Larson Scanner",
    "Comet",
    "Fireworks",
    "Fireworks Random",
    "Merry Christmas",
    "Fire Flicker",
    "Fire Flicker (soft)",
    "Fire Flicker (intense)",
    "Circus Combustus",
    "Halloween",
    "Bicolor Chase",
    "Tricolor Chase",
    "ICU",
    "Custom",
};

uint32_t SPEED_MAX = 65535;

// Macro to increase readability
#define SEGMENT fx->segments[fx->segment_index]
#define SEGMENT_RUNTIME fx->segment_runtimes[fx->segment_index]
#define SEGMENT_LENGTH (SEGMENT.stop - SEGMENT.start + 1)
#define RESET_RUNTIME                                                          \
    memset(fx->segment_runtimes, 0, sizeof(fx->segment_runtimes))

// segment runtime parameters
typedef struct Segment_runtime { // 16 bytes
    unsigned long next_time;
    uint32_t counter_mode_step;
    uint32_t counter_mode_call;
    uint16_t aux_param;
    uint16_t aux_param2;
} segment_runtime;

typedef struct WS2812FX {
    bool running;
    bool triggered;
    uint8_t brightness;
    uint8_t num_segments;
    uint8_t segment_index;
    segment segments[MAX_NUM_SEGMENTS];
    segment_runtime segment_runtimes[MAX_NUM_SEGMENTS];
} ws2812fx;

ws2812fx *fx;

void init_WS2812FX() {
    static bool init = false;
    if (init) {
        return;
    }
    RESET_RUNTIME;
    fx = malloc(sizeof(ws2812fx));
    if (fx == NULL) {
        return;
    }
    fx->brightness = DEFAULT_BRIGHTNESS;
    fx->running = false;
    fx->triggered = false;
    fx->num_segments = 1;

    for (int i = 0; i < MAX_NUM_SEGMENTS; i++) {
        fx->segments[i].mode = DEFAULT_MODE;
        fx->segments[i].start = 0;
        fx->segments[i].stop = NEOPIXEL_COUNT - 1;
        fx->segments[i].speed = DEFAULT_SPEED;
        for (int k = 0; k < NUM_COLORS; k++) {
            fx->segments[i].colors[k] = DEFAULT_COLOR;
        }
    }

    RESET_RUNTIME;
    nsec_neoPixel_init();
    setBrightness_WS2812FX(fx->brightness);
    nsec_neoPixel_show();
}

void service_WS2812FX() {
    if (fx->running || fx->triggered) {
        uint64_t now = get_current_time_millis();
        bool doShow = false;
        for (uint8_t i = 0; i < fx->num_segments; i++) {
            fx->segment_index = i;
            if (now > SEGMENT_RUNTIME.next_time || fx->triggered) {
                doShow = true;
                uint16_t delay = mode[SEGMENT.mode]();
                SEGMENT_RUNTIME.next_time = now + max((int)delay, SPEED_MIN);
                SEGMENT_RUNTIME.counter_mode_call++;
            }
        }
        if (doShow) {
            nrf_delay_ms(1);
            nsec_neoPixel_show();
        }
        fx->triggered = false;
    }
}

void start_WS2812FX() {
    RESET_RUNTIME;
    fx->running = true;
}

void stop_WS2812FX() {
    fx->running = false;
    strip_off_WS2812FX();
}

void trigger_WS2812FX() { fx->triggered = true; }

void setMode_WS2812FX(uint8_t m) {
    RESET_RUNTIME;
    fx->segments[0].mode = constrain(m, 0, MODE_COUNT - 1);
    setBrightness_WS2812FX(fx->brightness);
}

void setSegmentMode_WS2812FX(uint8_t segment_index, uint8_t m) {
    if (segment_index < fx->num_segments) {
        RESET_RUNTIME;
        fx->segments[segment_index].mode = constrain(m, 0, MODE_COUNT - 1);
        setBrightness_WS2812FX(fx->brightness);
    }
}

void setSpeed_WS2812FX(uint16_t s) {
    RESET_RUNTIME;
    fx->segments[0].speed = constrain(s, SPEED_MIN, SPEED_MAX);
}

void setReverse_WS2812FX(bool reverse) {
    RESET_RUNTIME;
    fx->segments[0].reverse = reverse;
}

void setSegmentReverse_WS2812FX(uint8_t segment_index, bool reverse) {
    if (segment_index < fx->num_segments) {
        RESET_RUNTIME;
        fx->segments[segment_index].reverse = reverse;
    }
}

bool getSegmentReverse_WS2812FX(uint8_t segment_index) {
    if (segment_index < fx->num_segments) {
        return fx->segments[segment_index].reverse;
    }
    return 0;
}

uint8_t getSegmentStart_WS2812FX(uint8_t segment_index) {
    if (segment_index < fx->num_segments) {
        return fx->segments[segment_index].start;
    }
    return 0;
}

void setSegmentStart_WS2812FX(uint8_t segment_index, uint8_t start) {
    if (segment_index < fx->num_segments) {
        fx->segments[segment_index].start = start;
    }
}
uint8_t getSegmentStop_WS2812FX(uint8_t segment_index) {
    if (segment_index < fx->num_segments) {
        return fx->segments[segment_index].stop;
    }
    return 0;
}

void setSegmentStop_WS2812FX(uint8_t segment_index, uint8_t stop) {
    if (segment_index < fx->num_segments) {
        fx->segments[segment_index].stop = stop;
    }
}

const char* getSegmentModeString_WS2812FX(uint8_t segment_index) {
    if (segment_index < fx->num_segments) {
        return name[fx->segments[segment_index].mode];
    }
    return "Unknown";
}

uint16_t getSegmentSpeed_WS2812FX(uint8_t segment_index) {
    if (segment_index < fx->num_segments) {
        return fx->segments[segment_index].speed;
    }
    return 0;
}

void setSegmentSpeed_WS2812FX(uint8_t segment_index, uint16_t segment_speed) {
    if (segment_index < fx->num_segments) {
        fx->segments[segment_index].speed = segment_speed;
    }
}

uint16_t getSegmentColor_WS2812FX(uint8_t segment_index, uint8_t color_index) {
    if (segment_index < fx->num_segments) {
        return fx->segments[segment_index].colors[color_index];
    }
    return 0;
}

void increaseSpeed_WS2812FX(uint8_t s) {
    uint16_t newSpeed = constrain(SEGMENT.speed + s, SPEED_MIN, SPEED_MAX);
    setSpeed_WS2812FX(newSpeed);
}

void decreaseSpeed_WS2812FX(uint8_t s) {
    uint16_t newSpeed = constrain(SEGMENT.speed - s, SPEED_MIN, SPEED_MAX);
    setSpeed_WS2812FX(newSpeed);
}

void setArrayColor_WS2812FX(uint8_t r, uint8_t g, uint8_t b, uint8_t index) {
    if (index < NUM_COLORS) {
        setArrayColor_packed_WS2812FX(
            ((uint32_t)r << 16) | ((uint32_t)g << 8) | b, index);
    }
}

void setColor_WS2812FX(uint8_t r, uint8_t g, uint8_t b) {
    setColor_packed_WS2812FX(((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void setArrayColor_packed_WS2812FX(uint32_t c, uint8_t index) {
    if (index < NUM_COLORS) {
        RESET_RUNTIME;
        fx->segments[0].colors[index] = c;
        setBrightness_WS2812FX(fx->brightness);
    }
}

void setSegmentArrayColor_packed_WS2812FX(uint8_t segment_index,
                                          uint8_t color_index, uint32_t c)
{
    if (color_index < NUM_COLORS && segment_index < fx->num_segments) {
        RESET_RUNTIME;
        fx->segments[segment_index].colors[color_index] = c;
        setBrightness_WS2812FX(fx->brightness);
    }
}

void setColor_packed_WS2812FX(uint32_t c) {
    RESET_RUNTIME;
    fx->segments[0].colors[0] = c;
    setBrightness_WS2812FX(fx->brightness);
}

void setBrightness_WS2812FX(uint8_t b) {
    fx->brightness = constrain(b, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    nsec_neoPixel_set_brightness(fx->brightness);
    nsec_neoPixel_show();
    nrf_delay_ms(1);
}

void increaseBrightness_WS2812FX(uint8_t s) {
    s = constrain(fx->brightness + s, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    setBrightness_WS2812FX(s);
}

void decreaseBrightness_WS2812FX(uint8_t s) {
    s = constrain(fx->brightness - s, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    setBrightness_WS2812FX(s);
}

bool isRunning_WS2812FX() { return fx->running; }

uint8_t getMode_WS2812FX(void) { return fx->segments[0].mode; }

uint16_t getSpeed_WS2812FX(void) { return fx->segments[0].speed; }

bool getReverse_WS2812FX(void) { return fx->segments[0].reverse; }

uint8_t getBrightness_WS2812FX(void) { return fx->brightness; }

uint16_t getLength_WS2812FX(void) {
    return fx->segments[0].stop - fx->segments[0].start + 1;
}

uint8_t getModeCount_WS2812FX(void) { return MODE_COUNT; }

uint8_t getNumSegments_WS2812FX(void) { return fx->num_segments; }

void setNumSegments_WS2812FX(uint8_t n) {
    RESET_RUNTIME;
    fx->num_segments = n;
}

uint32_t getColor_WS2812FX(void) { return fx->segments[0].colors[0]; }

uint32_t getArrayColor_WS2812FX(uint8_t index) {
    if (index < NUM_COLORS) {
        return fx->segments[0].colors[index];
    } else {
        return getColor_WS2812FX();
    }
}

const char *getModeName_WS2812FX(uint8_t m) {
    if (m < MODE_COUNT) {
        return name[m];
    } else {
        return "Unknown";
    }
}

void setSegment_WS2812FX(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode,
                         uint32_t color, uint16_t speed, bool reverse) {
    if (n < (sizeof(fx->segments) / sizeof(fx->segments[0]))) {
        if (n + 1 > fx->num_segments)
            fx->num_segments = n + 1;
        fx->segments[n].start = start;
        fx->segments[n].stop = stop;
        fx->segments[n].mode = mode;
        fx->segments[n].speed = speed;
        fx->segments[n].reverse = reverse;
        fx->segments[n].colors[0] = color;
    }
}

void setSegment_color_array_WS2812FX(uint8_t n, uint16_t start, uint16_t stop,
                                     uint8_t mode, const uint32_t colors[],
                                     uint16_t speed, bool reverse) {
    if (n < (sizeof(fx->segments) / sizeof(fx->segments[0]))) {
        if (n + 1 > fx->num_segments)
            fx->num_segments = n + 1;
        fx->segments[n].start = start;
        fx->segments[n].stop = stop;
        fx->segments[n].mode = mode;
        fx->segments[n].speed = speed;
        fx->segments[n].reverse = reverse;

        for (uint8_t i = 0; i < NUM_COLORS; i++) {
            fx->segments[n].colors[i] = colors[i];
        }
    }
}

void resetSegments_WS2812FX() {
    memset(fx->segments, 0, sizeof(fx->segments));
    memset(fx->segment_runtimes, 0, sizeof(fx->segment_runtimes));
    fx->segment_index = 0;
    fx->num_segments = 1;
    setSegment_WS2812FX(0, 0, NEOPIXEL_COUNT - 1, FX_MODE_STATIC, DEFAULT_COLOR,
                        DEFAULT_SPEED, false);
}

void moveSegment_WS2812FX(uint8_t src, uint8_t dest) {
    fx->segments[dest] = fx->segments[src];
}

/* #####################################################
#
#  Color and Blinken Functions
#
##################################################### */

/*
 * Turns everything off. Doh.
 */
void strip_off_WS2812FX() {
    nsec_neoPixel_clear();
    nsec_neoPixel_show();
}

/*
 * Put a value 0 to 255 in to get a color value.
 * The colours are a transition r -> g -> b -> back to r
 * Inspired by the Adafruit examples.
 */
uint32_t color_wheel(uint8_t pos) {
    pos = 255 - pos;
    if (pos < 85) {
        return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) |
               (pos * 3);
    } else if (pos < 170) {
        pos -= 85;
        return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) |
               (255 - pos * 3);
    } else {
        pos -= 170;
        return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) |
               (0);
    }
}

/*
 * Returns a new, random wheel index with a minimum distance of 42 from pos.
 */
uint8_t get_random_wheel_index(uint8_t pos) {
    uint8_t r = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t d = 0;

    while (d < 42) {
        r = nsec_random_get_byte(255);
        x = abs(pos - r);
        y = 255 - x;
        d = min(x, y);
    }

    return r;
}

/*
 * No blinking. Just plain old static light.
 */
uint16_t mode_static(void) {
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, SEGMENT.colors[0]);
    }
    return 500;
}

/*
 * Blink/strobe function
 * Alternate between color1 and color2
 * if(strobe == true) then create a strobe effect
 */

uint16_t blink(uint32_t color1, uint32_t color2, bool strobe) {
    uint32_t color =
        ((SEGMENT_RUNTIME.counter_mode_call & 1) == 0) ? color1 : color2;
    if (SEGMENT.reverse)
        color = (color == color1) ? color2 : color1;

    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, color);
    }

    if ((SEGMENT_RUNTIME.counter_mode_call & 1) == 0) {
        return strobe ? 20 : (SEGMENT.speed / 2);
    } else {
        return strobe ? SEGMENT.speed - 20 : (SEGMENT.speed / 2);
    }
}

/*
 * Normal blinking. 50% on/off time.
 */
uint16_t mode_blink(void) {
    return blink(SEGMENT.colors[0], SEGMENT.colors[1], false);
}

/*
 * Classic Blink effect. Cycling through the rainbow.
 */
uint16_t mode_blink_rainbow(void) {
    return blink(color_wheel(SEGMENT_RUNTIME.counter_mode_call & 0xFF),
                 SEGMENT.colors[1], false);
}

/*
 * Classic Strobe effect.
 */
uint16_t mode_strobe(void) {
    return blink(SEGMENT.colors[0], SEGMENT.colors[1], true);
}

/*
 * Classic Strobe effect. Cycling through the rainbow.
 */
uint16_t mode_strobe_rainbow(void) {
    return blink(color_wheel(SEGMENT_RUNTIME.counter_mode_call & 0xFF),
                 SEGMENT.colors[1], true);
}

/*
 * Color wipe function
 * LEDs are turned on (color1) in sequence, then turned off (color2) in
 * sequence. if (bool rev == true) then LEDs are turned off in reverse order
 */
uint16_t color_wipe(uint32_t color1, uint32_t color2, bool rev) {
    if (SEGMENT_RUNTIME.counter_mode_step < SEGMENT_LENGTH) {
        uint32_t led_offset = SEGMENT_RUNTIME.counter_mode_step;
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - led_offset,
                                                 color1);
        } else {
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + led_offset,
                                                 color1);
        }
    } else {
        uint32_t led_offset =
            SEGMENT_RUNTIME.counter_mode_step - SEGMENT_LENGTH;
        if ((SEGMENT.reverse && !rev) || (!SEGMENT.reverse && rev)) {
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - led_offset,
                                                 color2);
        } else {
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + led_offset,
                                                 color2);
        }
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % (SEGMENT_LENGTH * 2);
    return (SEGMENT.speed / (SEGMENT_LENGTH * 2));
}

/*
 * Lights all LEDs one after another.
 */
uint16_t mode_color_wipe(void) {
    return color_wipe(SEGMENT.colors[0], SEGMENT.colors[1], false);
}

uint16_t mode_color_wipe_inv(void) {
    return color_wipe(SEGMENT.colors[1], SEGMENT.colors[0], false);
}

uint16_t mode_color_wipe_rev(void) {
    return color_wipe(SEGMENT.colors[0], SEGMENT.colors[1], true);
}

uint16_t mode_color_wipe_rev_inv(void) {
    return color_wipe(SEGMENT.colors[1], SEGMENT.colors[0], true);
}

/*
 * Turns all LEDs after each other to a random color.
 * Then starts over with another color.
 */
uint16_t mode_color_wipe_random(void) {
    if (SEGMENT_RUNTIME.counter_mode_step % SEGMENT_LENGTH ==
        0) { // aux_param will store our random color wheel index
        SEGMENT_RUNTIME.aux_param =
            get_random_wheel_index(SEGMENT_RUNTIME.aux_param);
    }
    uint32_t color = color_wheel(SEGMENT_RUNTIME.aux_param);
    return color_wipe(color, color, false) * 2;
}

/*
 * Random color introduced alternating from start and end of strip.
 */
uint16_t mode_color_sweep_random(void) {
    if (SEGMENT_RUNTIME.counter_mode_step % SEGMENT_LENGTH ==
        0) { // aux_param will store our random color wheel index
        SEGMENT_RUNTIME.aux_param =
            get_random_wheel_index(SEGMENT_RUNTIME.aux_param);
    }
    uint32_t color = color_wheel(SEGMENT_RUNTIME.aux_param);
    return color_wipe(color, color, true) * 2;
}

/*
 * Lights all LEDs in one random color up. Then switches them
 * to the next random color.
 */
uint16_t mode_random_color(void) {
    SEGMENT_RUNTIME.aux_param = get_random_wheel_index(
        SEGMENT_RUNTIME
            .aux_param); // aux_param will store our random color wheel index
    uint32_t color = color_wheel(SEGMENT_RUNTIME.aux_param);

    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, color);
    }
    return (SEGMENT.speed);
}

/*
 * Lights every LED in a random color. Changes one random LED after the other
 * to another random color.
 */
uint16_t mode_single_dynamic(void) {
    if (SEGMENT_RUNTIME.counter_mode_call == 0) {
        for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
            nsec_neoPixel_set_pixel_color_packed(
                i, color_wheel(nsec_random_get_byte(255)));
        }
    }
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1),
        color_wheel(nsec_random_get_byte(255)));
    return (SEGMENT.speed);
}

/*
 * Lights every LED in a random color. Changes all LED at the same time
 * to new random colors.
 */
uint16_t mode_multi_dynamic(void) {
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(
            i, color_wheel(nsec_random_get_byte(255)));
    }
    return (SEGMENT.speed);
}

/*
 * Does the "standby-breathing" of well known i-Devices. Fixed Speed.
 * Use mode "fade" if you like to have something similar with a different speed.
 */
uint16_t mode_breath(void) {
    //                                      0    1    2   3   4   5   6    7   8
    //                                      9  10  11   12   13   14   15   16
    //                                      // step
    uint16_t breath_delay_steps[] = {
        7,  9,  13, 15, 16, 17, 18, 930, 19,
        18, 15, 13, 9,  7,  4,  5,  10}; // magic numbers for breathing LED
    uint8_t breath_brightness_steps[] = {
        150, 125, 100, 75,  50,  25,  16,  15, 16,
        25,  50,  75,  100, 125, 150, 220, 255}; // even more magic numbers!

    if (SEGMENT_RUNTIME.counter_mode_call == 0) {
        SEGMENT_RUNTIME.aux_param =
            breath_brightness_steps[0] +
            1; // we use aux_param to store the brightness
    }

    uint8_t breath_brightness = SEGMENT_RUNTIME.aux_param;

    if (SEGMENT_RUNTIME.counter_mode_step < 8) {
        breath_brightness--;
    } else {
        breath_brightness++;
    }

    // update index of current delay when target brightness is reached, start
    // over after the last step
    if (breath_brightness ==
        breath_brightness_steps[SEGMENT_RUNTIME.counter_mode_step]) {
        SEGMENT_RUNTIME.counter_mode_step =
            (SEGMENT_RUNTIME.counter_mode_step + 1) %
            (sizeof(breath_brightness_steps) / sizeof(uint8_t));
    }

    int lum =
        map(breath_brightness, 0, 255, 0,
            fx->brightness); // keep luminosity below brightness set by user
    // uint8_t w = (SEGMENT.colors[0] >> 24 & 0xFF) * lum / _brightness; //
    // modify RGBW colors with brightness info
    uint8_t r = (SEGMENT.colors[0] >> 16 & 0xFF) * lum / fx->brightness;
    uint8_t g = (SEGMENT.colors[0] >> 8 & 0xFF) * lum / fx->brightness;
    uint8_t b = (SEGMENT.colors[0] & 0xFF) * lum / fx->brightness;
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color(i, r, g, b);
    }

    SEGMENT_RUNTIME.aux_param = breath_brightness;
    return breath_delay_steps[SEGMENT_RUNTIME.counter_mode_step];
}

/*
 * Fades the LEDs on and (almost) off again.
 */
uint16_t mode_fade(void) {
    int lum = SEGMENT_RUNTIME.counter_mode_step - 31;
    lum = 63 - (abs(lum) * 2);
    lum = map(lum, 0, 64, min(25, (int)fx->brightness), fx->brightness);

    // uint8_t w = (SEGMENT.colors[0] >> 24 & 0xFF) * lum / _brightness; //
    // modify RGBW colors with brightness info
    uint8_t r = (SEGMENT.colors[0] >> 16 & 0xFF) * lum / fx->brightness;
    uint8_t g = (SEGMENT.colors[0] >> 8 & 0xFF) * lum / fx->brightness;
    uint8_t b = (SEGMENT.colors[0] & 0xFF) * lum / fx->brightness;
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color(i, r, g, b);
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % 64;
    return (SEGMENT.speed / 64);
}

/*
 * Runs a single pixel back and forth.
 */
uint16_t mode_scan(void) {
    if (SEGMENT_RUNTIME.counter_mode_step > (SEGMENT_LENGTH * 2) - 3) {
        SEGMENT_RUNTIME.counter_mode_step = 0;
    }

    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, BLACK);
    }

    int led_offset = SEGMENT_RUNTIME.counter_mode_step - (SEGMENT_LENGTH - 1);
    led_offset = abs(led_offset);

    if (SEGMENT.reverse) {
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - led_offset,
                                             SEGMENT.colors[0]);
    } else {
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + led_offset,
                                             SEGMENT.colors[0]);
    }

    SEGMENT_RUNTIME.counter_mode_step++;
    return (SEGMENT.speed / (SEGMENT_LENGTH * 2));
}

/*
 * Runs two pixel back and forth in opposite directions.
 */
uint16_t mode_dual_scan(void) {
    if (SEGMENT_RUNTIME.counter_mode_step > (SEGMENT_LENGTH * 2) - 3) {
        SEGMENT_RUNTIME.counter_mode_step = 0;
    }

    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, BLACK);
    }

    int led_offset = SEGMENT_RUNTIME.counter_mode_step - (SEGMENT_LENGTH - 1);
    led_offset = abs(led_offset);

    nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + led_offset,
                                         SEGMENT.colors[0]);
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + SEGMENT_LENGTH - led_offset - 1, SEGMENT.colors[0]);

    SEGMENT_RUNTIME.counter_mode_step++;
    return (SEGMENT.speed / (SEGMENT_LENGTH * 2));
}

/*
 * Cycles all LEDs at once through a rainbow.
 */
uint16_t mode_rainbow(void) {
    uint32_t color = color_wheel(SEGMENT_RUNTIME.counter_mode_step);
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, color);
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) & 0xFF;
    return (SEGMENT.speed / 256);
}

/*
 * Cycles a rainbow over the entire string of LEDs.
 */
uint16_t mode_rainbow_cycle(void) {
    for (uint16_t i = 0; i < SEGMENT_LENGTH; i++) {
        uint32_t color = color_wheel(
            ((i * 256 / SEGMENT_LENGTH) + SEGMENT_RUNTIME.counter_mode_step) &
            0xFF);
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color);
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) & 0xFF;
    return (SEGMENT.speed / 256);
}

/*
 * theater chase function
 */
uint16_t theater_chase(uint32_t color1, uint32_t color2) {
    SEGMENT_RUNTIME.counter_mode_call = SEGMENT_RUNTIME.counter_mode_call % 3;
    for (uint16_t i = 0; i < SEGMENT_LENGTH; i++) {
        if ((i % 3) == SEGMENT_RUNTIME.counter_mode_call) {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color1);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color1);
            }
        } else {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color2);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color2);
            }
        }
    }

    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Theatre-style crawling lights.
 * Inspired by the Adafruit examples.
 */
uint16_t mode_theater_chase(void) {
    return theater_chase(SEGMENT.colors[0], BLACK);
}

/*
 * Theatre-style crawling lights with rainbow effect.
 * Inspired by the Adafruit examples.
 */
uint16_t mode_theater_chase_rainbow(void) {
    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) & 0xFF;
    return theater_chase(color_wheel(SEGMENT_RUNTIME.counter_mode_step), BLACK);
}

/*
 * Running lights effect with smooth sine transition.
 */
uint16_t mode_running_lights(void) {
    // uint8_t w = ((SEGMENT.colors[0] >> 24) & 0xFF);
    uint8_t r = ((SEGMENT.colors[0] >> 16) & 0xFF);
    uint8_t g = ((SEGMENT.colors[0] >> 8) & 0xFF);
    uint8_t b = (SEGMENT.colors[0] & 0xFF);

    float radPerLed = (2.0 * 3.14159) / SEGMENT_LENGTH;
    for (uint16_t i = 0; i < SEGMENT_LENGTH; i++) {
        int lum =
            map((int)(arm_sin_f32((i + SEGMENT_RUNTIME.counter_mode_step) *
                                  radPerLed) *
                      128),
                -128, 128, 0, 255);
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color(SEGMENT.start + i, (r * lum) / 256,
                                          (g * lum) / 256, (b * lum) / 256);
        } else {
            nsec_neoPixel_set_pixel_color(SEGMENT.stop - i, (r * lum) / 256,
                                          (g * lum) / 256, (b * lum) / 256);
        }
    }
    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * twinkle function
 */
uint16_t twinkle(uint32_t color) {
    if (SEGMENT_RUNTIME.counter_mode_step == 0) {
        for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
            nsec_neoPixel_set_pixel_color_packed(i, BLACK);
        }
        uint16_t min_leds =
            max(1, SEGMENT_LENGTH / 5); // make sure, at least one LED is on
        uint16_t max_leds =
            max(1, SEGMENT_LENGTH / 2); // make sure, at least one LED is on
        SEGMENT_RUNTIME.counter_mode_step =
            nsec_random_get_byte_range(min_leds, max_leds - 1);
    }

    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1), color);

    SEGMENT_RUNTIME.counter_mode_step--;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Blink several LEDs on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
uint16_t mode_twinkle(void) { return twinkle(SEGMENT.colors[0]); }

/*
 * Blink several LEDs in random colors on, reset, repeat.
 * Inspired by www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
uint16_t mode_twinkle_random(void) {
    return twinkle(color_wheel(nsec_random_get_byte(255)));
}

/*
 * fade out function
 * fades out the current segment by dividing each pixel's intensity by 2
 */
void fade_out() {
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        uint32_t color = nsec_neoPixel_get_pixel_color(i);
        color = (color >> 1) & 0x7F7F7F7F;
        nsec_neoPixel_set_pixel_color_packed(i, color);
    }
}

/*
 * twinkle_fade function
 */
uint16_t twinkle_fade(uint32_t color) {
    fade_out();

    if (nsec_random_get_byte(2) == 0) {
        nsec_neoPixel_set_pixel_color_packed(
            SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1), color);
    }
    return (SEGMENT.speed / 8);
}

/*
 * Blink several LEDs on, fading out.
 */
uint16_t mode_twinkle_fade(void) { return twinkle_fade(SEGMENT.colors[0]); }

/*
 * Blink several LEDs in random colors on, fading out.
 */
uint16_t mode_twinkle_fade_random(void) {
    return twinkle_fade(color_wheel(nsec_random_get_byte(255)));
}

/*
 * Blinks one LED at a time.
 * Inspired by www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
uint16_t mode_sparkle(void) {
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + SEGMENT_RUNTIME.aux_param, BLACK);
    SEGMENT_RUNTIME.aux_param = nsec_random_get_byte(
        SEGMENT_LENGTH - 1); // aux_param stores the random led index
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + SEGMENT_RUNTIME.aux_param, SEGMENT.colors[0]);
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Lights all LEDs in the color. Flashes single white pixels randomly.
 * Inspired by www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
uint16_t mode_flash_sparkle(void) {
    if (SEGMENT_RUNTIME.counter_mode_call == 0) {
        for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
            nsec_neoPixel_set_pixel_color_packed(i, SEGMENT.colors[0]);
        }
    }

    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + SEGMENT_RUNTIME.aux_param, SEGMENT.colors[0]);

    if (nsec_random_get_byte(4) == 0) {
        SEGMENT_RUNTIME.aux_param = nsec_random_get_byte(
            SEGMENT_LENGTH - 1); // aux_param stores the random led index
        nsec_neoPixel_set_pixel_color_packed(
            SEGMENT.start + SEGMENT_RUNTIME.aux_param, WHITE);
        return 20;
    }
    return SEGMENT.speed;
}

/*
 * Like flash sparkle. With more flash.
 * Inspired by www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 */
uint16_t mode_hyper_sparkle(void) {
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, SEGMENT.colors[0]);
    }

    if (nsec_random_get_byte(4) < 2) {
        for (uint16_t i = 0; i < max(1, SEGMENT_LENGTH / 3); i++) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1),
                WHITE);
        }
        return 20;
    }
    return SEGMENT.speed;
}

/*
 * Strobe effect with different strobe count and pause, controlled by speed.
 */
uint16_t mode_multi_strobe(void) {
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, BLACK);
    }

    uint16_t delay = SEGMENT.speed / (2 * ((SEGMENT.speed / 10) + 1));
    if (SEGMENT_RUNTIME.counter_mode_step < (2 * ((SEGMENT.speed / 10) + 1))) {
        if ((SEGMENT_RUNTIME.counter_mode_step & 1) == 0) {
            for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
                nsec_neoPixel_set_pixel_color_packed(i, SEGMENT.colors[0]);
            }
            delay = 20;
        } else {
            delay = 50;
        }
    }
    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) %
        ((2 * ((SEGMENT.speed / 10) + 1)) + 1);
    return delay;
}

/*
 * color chase function.
 * color1 = background color
 * color2 and color3 = colors of two adjacent leds
 */

uint16_t chase(uint32_t color1, uint32_t color2, uint32_t color3) {
    uint16_t a = SEGMENT_RUNTIME.counter_mode_step;
    uint16_t b = (a + 1) % SEGMENT_LENGTH;
    uint16_t c = (b + 1) % SEGMENT_LENGTH;
    if (SEGMENT.reverse) {
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - a, color1);
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - b, color2);
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - c, color3);
    } else {
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + a, color1);
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + b, color2);
        nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + c, color3);
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Bicolor chase mode
 */
uint16_t mode_bicolor_chase(void) {
    return chase(SEGMENT.colors[0], SEGMENT.colors[1], SEGMENT.colors[2]);
}

/*
 * White running on _color.
 */
uint16_t mode_chase_color(void) {
    return chase(SEGMENT.colors[0], WHITE, WHITE);
}

/*
 * Black running on _color.
 */
uint16_t mode_chase_blackout(void) {
    return chase(SEGMENT.colors[0], BLACK, BLACK);
}

/*
 * _color running on white.
 */
uint16_t mode_chase_white(void) {
    return chase(WHITE, SEGMENT.colors[0], SEGMENT.colors[0]);
}

/*
 * White running followed by random color.
 */
uint16_t mode_chase_random(void) {
    if (SEGMENT_RUNTIME.counter_mode_step == 0) {
        SEGMENT_RUNTIME.aux_param =
            get_random_wheel_index(SEGMENT_RUNTIME.aux_param);
    }
    return chase(color_wheel(SEGMENT_RUNTIME.aux_param), WHITE, WHITE);
}

/*
 * Rainbow running on white.
 */
uint16_t mode_chase_rainbow_white(void) {
    uint16_t n = SEGMENT_RUNTIME.counter_mode_step;
    uint16_t m = (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
    uint32_t color2 = color_wheel(((n * 256 / SEGMENT_LENGTH) +
                                   (SEGMENT_RUNTIME.counter_mode_call & 0xFF)) &
                                  0xFF);
    uint32_t color3 = color_wheel(((m * 256 / SEGMENT_LENGTH) +
                                   (SEGMENT_RUNTIME.counter_mode_call & 0xFF)) &
                                  0xFF);

    return chase(WHITE, color2, color3);
}

/*
 * White running on rainbow.
 */
uint16_t mode_chase_rainbow(void) {
    uint8_t color_sep = 256 / SEGMENT_LENGTH;
    uint8_t color_index = SEGMENT_RUNTIME.counter_mode_call & 0xFF;
    uint32_t color = color_wheel(
        ((SEGMENT_RUNTIME.counter_mode_step * color_sep) + color_index) & 0xFF);

    return chase(color, WHITE, WHITE);
}

/*
 * Black running on rainbow.
 */
uint16_t mode_chase_blackout_rainbow(void) {
    uint8_t color_sep = 256 / SEGMENT_LENGTH;
    uint8_t color_index = SEGMENT_RUNTIME.counter_mode_call & 0xFF;
    uint32_t color = color_wheel(
        ((SEGMENT_RUNTIME.counter_mode_step * color_sep) + color_index) & 0xFF);

    return chase(color, BLACK, BLACK);
}

/*
 * White flashes running on _color.
 */
uint16_t mode_chase_flash(void) {
    const static uint8_t flash_count = 4;
    uint8_t flash_step =
        SEGMENT_RUNTIME.counter_mode_call % ((flash_count * 2) + 1);

    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        nsec_neoPixel_set_pixel_color_packed(i, SEGMENT.colors[0]);
    }

    uint16_t delay = (SEGMENT.speed / SEGMENT_LENGTH);
    if (flash_step < (flash_count * 2)) {
        if (flash_step % 2 == 0) {
            uint16_t n = SEGMENT_RUNTIME.counter_mode_step;
            uint16_t m =
                (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - n, WHITE);
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - m, WHITE);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + n, WHITE);
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + m, WHITE);
            }
            delay = 20;
        } else {
            delay = 30;
        }
    } else {
        SEGMENT_RUNTIME.counter_mode_step =
            (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
    }
    return delay;
}

/*
 * White flashes running, followed by random color.
 */
uint16_t mode_chase_flash_random(void) {
    const static uint8_t flash_count = 4;
    uint8_t flash_step =
        SEGMENT_RUNTIME.counter_mode_call % ((flash_count * 2) + 1);

    for (uint16_t i = 0; i < SEGMENT_RUNTIME.counter_mode_step; i++) {
        nsec_neoPixel_set_pixel_color_packed(
            SEGMENT.start + i, color_wheel(SEGMENT_RUNTIME.aux_param));
    }

    uint16_t delay = (SEGMENT.speed / SEGMENT_LENGTH);
    if (flash_step < (flash_count * 2)) {
        uint16_t n = SEGMENT_RUNTIME.counter_mode_step;
        uint16_t m = (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
        if (flash_step % 2 == 0) {
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + n, WHITE);
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + m, WHITE);
            delay = 20;
        } else {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + n, color_wheel(SEGMENT_RUNTIME.aux_param));
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + m, BLACK);
            delay = 30;
        }
    } else {
        SEGMENT_RUNTIME.counter_mode_step =
            (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;

        if (SEGMENT_RUNTIME.counter_mode_step == 0) {
            SEGMENT_RUNTIME.aux_param =
                get_random_wheel_index(SEGMENT_RUNTIME.aux_param);
        }
    }
    return delay;
}

/*
 * Alternating pixels running function.
 */
uint16_t running(uint32_t color1, uint32_t color2) {
    for (uint16_t i = 0; i < SEGMENT_LENGTH; i++) {
        if ((i + SEGMENT_RUNTIME.counter_mode_step) % 4 < 2) {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color1);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color1);
            }
        } else {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color2);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color2);
            }
        }
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) & 0x3;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Alternating color/white pixels running.
 */
uint16_t mode_running_color(void) { return running(SEGMENT.colors[0], WHITE); }

/*
 * Alternating red/blue pixels running.
 */
uint16_t mode_running_red_blue(void) { return running(RED, BLUE); }

/*
 * Alternating red/green pixels running.
 */
uint16_t mode_merry_christmas(void) { return running(RED, GREEN); }

/*
 * Alternating orange/purple pixels running.
 */
uint16_t mode_halloween(void) { return running(PURPLE, ORANGE); }

/*
 * Random colored pixels running.
 */
uint16_t mode_running_random(void) {
    for (uint16_t i = SEGMENT_LENGTH - 1; i > 0; i--) {
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.stop - i,
                nsec_neoPixel_get_pixel_color(SEGMENT.stop - i + 1));
        } else {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + i,
                nsec_neoPixel_get_pixel_color(SEGMENT.start + i - 1));
        }
    }

    if (SEGMENT_RUNTIME.counter_mode_step == 0) {
        SEGMENT_RUNTIME.aux_param =
            get_random_wheel_index(SEGMENT_RUNTIME.aux_param);
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.stop, color_wheel(SEGMENT_RUNTIME.aux_param));
        } else {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start, color_wheel(SEGMENT_RUNTIME.aux_param));
        }
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step == 0) ? 1 : 0;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * K.I.T.T.
 */
uint16_t mode_larson_scanner(void) {
    fade_out();

    if (SEGMENT_RUNTIME.counter_mode_step < SEGMENT_LENGTH) {
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.stop - SEGMENT_RUNTIME.counter_mode_step,
                SEGMENT.colors[0]);
        } else {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + SEGMENT_RUNTIME.counter_mode_step,
                SEGMENT.colors[0]);
        }
    } else {
        if (SEGMENT.reverse) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.stop -
                    ((SEGMENT_LENGTH * 2) - SEGMENT_RUNTIME.counter_mode_step) +
                    2,
                SEGMENT.colors[0]);
        } else {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start +
                    ((SEGMENT_LENGTH * 2) - SEGMENT_RUNTIME.counter_mode_step) -
                    2,
                SEGMENT.colors[0]);
        }
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % ((SEGMENT_LENGTH * 2) - 2);
    return (SEGMENT.speed / (SEGMENT_LENGTH * 2));
}

/*
 * Firing comets from one end.
 */
uint16_t mode_comet(void) {
    fade_out();

    if (SEGMENT.reverse) {
        nsec_neoPixel_set_pixel_color_packed(
            SEGMENT.stop - SEGMENT_RUNTIME.counter_mode_step,
            SEGMENT.colors[0]);
    } else {
        nsec_neoPixel_set_pixel_color_packed(
            SEGMENT.start + SEGMENT_RUNTIME.counter_mode_step,
            SEGMENT.colors[0]);
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % SEGMENT_LENGTH;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Fireworks function.
 */
uint16_t fireworks(uint32_t color) {
    fade_out();
    uint32_t prevLed, thisLed, nextLed;

    // set brightness(i) = ((brightness(i-1)/2 + brightness(i+1)) / 2) +
    // brightness(i)
    for (uint16_t i = SEGMENT.start + 1; i < SEGMENT.stop; i++) {
        // the new way. not as precise, but much faster, smaller and works with
        // RGBW leds
        prevLed = (nsec_neoPixel_get_pixel_color(i - 1) >> 2) & 0x3F3F3F3F;
        thisLed = nsec_neoPixel_get_pixel_color(i);
        nextLed = (nsec_neoPixel_get_pixel_color(i + 1) >> 2) & 0x3F3F3F3F;
        nsec_neoPixel_set_pixel_color_packed(i, prevLed + thisLed + nextLed);
    }

    if (!fx->triggered) {
        for (uint16_t i = 0; i < max(1, SEGMENT_LENGTH / 20); i++) {
            if (nsec_random_get_byte(9) == 0) {
                nsec_neoPixel_set_pixel_color_packed(
                    SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1),
                    color);
            }
        }
    } else {
        for (uint16_t i = 0; i < max(1, SEGMENT_LENGTH / 10); i++) {
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + nsec_random_get_byte(SEGMENT_LENGTH - 1),
                color);
        }
    }
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Firework sparks.
 */
uint16_t mode_fireworks(void) {
    uint32_t color = SEGMENT.colors[0];
    return fireworks(color);
}

/*
 * Random colored firework sparks.
 */
uint16_t mode_fireworks_random(void) {
    uint32_t color = color_wheel(nsec_random_get_byte(255));
    return fireworks(color);
}

/*
 * Fire flicker function
 */
uint16_t fire_flicker(int rev_intensity) {
    // byte w = (SEGMENT.colors[0] >> 24) & 0xFF;
    uint8_t r = (SEGMENT.colors[0] >> 16) & 0xFF;
    uint8_t g = (SEGMENT.colors[0] >> 8) & 0xFF;
    uint8_t b = (SEGMENT.colors[0] & 0xFF);
    uint8_t lum = max(r, max(g, b)) / rev_intensity;
    for (uint16_t i = SEGMENT.start; i <= SEGMENT.stop; i++) {
        int flicker = nsec_random_get_byte_range(0, lum - 1);
        nsec_neoPixel_set_pixel_color(i, max(r - flicker, 0),
                                      max(g - flicker, 0), max(b - flicker, 0));
    }
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Random flickering.
 */
uint16_t mode_fire_flicker(void) { return fire_flicker(3); }

/*
 * Random flickering, less intensity.
 */
uint16_t mode_fire_flicker_soft(void) { return fire_flicker(6); }

/*
 * Random flickering, more intensity.
 */
uint16_t mode_fire_flicker_intense(void) { return fire_flicker(1.7); }

/*
 * Tricolor chase function
 */
uint16_t tricolor_chase(uint32_t color1, uint32_t color2, uint32_t color3) {
    for (uint16_t i = 0; i < SEGMENT_LENGTH; i++) {
        if ((i + SEGMENT_RUNTIME.counter_mode_step) % 6 < 2) {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color1);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color1);
            }
        } else if ((i + SEGMENT_RUNTIME.counter_mode_step) % 6 < 4) {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color2);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color2);
            }
        } else {
            if (SEGMENT.reverse) {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + i, color3);
            } else {
                nsec_neoPixel_set_pixel_color_packed(SEGMENT.stop - i, color3);
            }
        }
    }

    SEGMENT_RUNTIME.counter_mode_step =
        (SEGMENT_RUNTIME.counter_mode_step + 1) % 6;
    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Tricolor chase mode
 */
uint16_t mode_tricolor_chase(void) {
    return tricolor_chase(SEGMENT.colors[0], SEGMENT.colors[1],
                          SEGMENT.colors[2]);
}

/*
 * Alternating white/red/black pixels running.
 */
uint16_t mode_circus_combustus(void) {
    return tricolor_chase(RED, WHITE, BLACK);
}

/*
 * ICU mode
 */
uint16_t mode_icu(void) {
    uint16_t dest = SEGMENT_RUNTIME.counter_mode_step & 0xFFFF;

    nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + dest,
                                         SEGMENT.colors[0]);
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + dest + SEGMENT_LENGTH / 2, SEGMENT.colors[0]);

    if (SEGMENT_RUNTIME.aux_param == dest) { // pause between eye movements
        if (nsec_random_get_byte(5) == 0) {  // blink once in a while
            nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + dest, 0);
            nsec_neoPixel_set_pixel_color_packed(
                SEGMENT.start + dest + SEGMENT_LENGTH / 2, 0);
            return 200;
        }
        SEGMENT_RUNTIME.aux_param =
            nsec_random_get_byte((SEGMENT_LENGTH / 2) - 1);
        return 1000 + nsec_random_get_u16(1999);
    }

    nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + dest, 0);
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + dest + SEGMENT_LENGTH / 2, 0);

    if (SEGMENT_RUNTIME.aux_param > SEGMENT_RUNTIME.counter_mode_step) {
        SEGMENT_RUNTIME.counter_mode_step++;
        dest++;
    } else if (SEGMENT_RUNTIME.aux_param < SEGMENT_RUNTIME.counter_mode_step) {
        SEGMENT_RUNTIME.counter_mode_step--;
        dest--;
    }

    nsec_neoPixel_set_pixel_color_packed(SEGMENT.start + dest,
                                         SEGMENT.colors[0]);
    nsec_neoPixel_set_pixel_color_packed(
        SEGMENT.start + dest + SEGMENT_LENGTH / 2, SEGMENT.colors[0]);

    return (SEGMENT.speed / SEGMENT_LENGTH);
}

/*
 * Custom mode
 */
uint16_t (*customMode)(void) = NULL;
uint16_t mode_custom() {
    if (customMode == NULL) {
        return 1000; // if custom mode not set, do nothing
    } else {
        return customMode();
    }
}

/*
 * Custom mode helper
 */
void setCustomMode(uint16_t (*p)()) { customMode = p; }
