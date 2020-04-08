/*
  WS2812FX.h - Library for WS2812 LED effects.

  This is a C version of the arduino library:
  https://github.com/kitesurfer1404/WS2812FX

  That library use led_effects instead of Adafruit_NeoPixel

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
*/

#ifndef WS2812FX_h
#define WS2812FX_h

#include "app/random.h"
#include "led_effects.h"
#include <stdbool.h>

#define DEFAULT_BRIGHTNESS 50
#define DEFAULT_MODE 0
#define DEFAULT_SPEED 1000
#define DEFAULT_COLOR 0x000000

#define SPEED_MIN 10

#define BRIGHTNESS_MIN 0
#define BRIGHTNESS_MAX 255

/* each segment uses 36 bytes of SRAM memory, so if you're application fails
  because of insufficient memory, decreasing MAX_NUM_SEGMENTS may help */
#define MAX_NUM_SEGMENTS NEOPIXEL_COUNT
#define NUM_COLORS 3 /* number of colors per segment */

// some common colors
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define YELLOW 0xFFFF00
#define CYAN 0x00FFFF
#define MAGENTA 0xFF00FF
#define PURPLE 0x400080
#define ORANGE 0xFF3000
#define ULTRAWHITE 0xFFFFFFFF

#define MODE_COUNT 57

#define FX_MODE_STATIC 0
#define FX_MODE_BLINK 1
#define FX_MODE_BREATH 2
#define FX_MODE_COLOR_WIPE 3
#define FX_MODE_COLOR_WIPE_INV 4
#define FX_MODE_COLOR_WIPE_REV 5
#define FX_MODE_COLOR_WIPE_REV_INV 6
#define FX_MODE_COLOR_WIPE_RANDOM 7
#define FX_MODE_RANDOM_COLOR 8
#define FX_MODE_SINGLE_DYNAMIC 9
#define FX_MODE_MULTI_DYNAMIC 10
#define FX_MODE_RAINBOW 11
#define FX_MODE_RAINBOW_CYCLE 12
#define FX_MODE_SCAN 13
#define FX_MODE_DUAL_SCAN 14
#define FX_MODE_FADE 15
#define FX_MODE_THEATER_CHASE 16
#define FX_MODE_THEATER_CHASE_RAINBOW 17
#define FX_MODE_RUNNING_LIGHTS 18
#define FX_MODE_TWINKLE 19
#define FX_MODE_TWINKLE_RANDOM 20
#define FX_MODE_TWINKLE_FADE 21
#define FX_MODE_TWINKLE_FADE_RANDOM 22
#define FX_MODE_SPARKLE 23
#define FX_MODE_FLASH_SPARKLE 24
#define FX_MODE_HYPER_SPARKLE 25
#define FX_MODE_STROBE 26
#define FX_MODE_STROBE_RAINBOW 27
#define FX_MODE_MULTI_STROBE 28
#define FX_MODE_BLINK_RAINBOW 29
#define FX_MODE_CHASE_WHITE 30
#define FX_MODE_CHASE_COLOR 31
#define FX_MODE_CHASE_RANDOM 32
#define FX_MODE_CHASE_RAINBOW 33
#define FX_MODE_CHASE_FLASH 34
#define FX_MODE_CHASE_FLASH_RANDOM 35
#define FX_MODE_CHASE_RAINBOW_WHITE 36
#define FX_MODE_CHASE_BLACKOUT 37
#define FX_MODE_CHASE_BLACKOUT_RAINBOW 38
#define FX_MODE_COLOR_SWEEP_RANDOM 39
#define FX_MODE_RUNNING_COLOR 40
#define FX_MODE_RUNNING_RED_BLUE 41
#define FX_MODE_RUNNING_RANDOM 42
#define FX_MODE_LARSON_SCANNER 43
#define FX_MODE_COMET 44
#define FX_MODE_FIREWORKS 45
#define FX_MODE_FIREWORKS_RANDOM 46
#define FX_MODE_MERRY_CHRISTMAS 47
#define FX_MODE_FIRE_FLICKER 48
#define FX_MODE_FIRE_FLICKER_SOFT 49
#define FX_MODE_FIRE_FLICKER_INTENSE 50
#define FX_MODE_CIRCUS_COMBUSTUS 51
#define FX_MODE_HALLOWEEN 52
#define FX_MODE_BICOLOR_CHASE 53
#define FX_MODE_TRICOLOR_CHASE 54
#define FX_MODE_ICU 55
#define FX_MODE_CUSTOM 56

// segment parameters
typedef struct Segment { // 20 bytes
    uint16_t start;
    uint16_t stop;
    uint16_t speed;
    uint8_t mode;
    bool reverse;
    uint32_t colors[3];
}__attribute__((packed))segment;

void init_WS2812FX(void);
void service_WS2812FX(void);
void start_WS2812FX(void);
void stop_WS2812FX(void);
void setMode_WS2812FX(uint8_t m);
void setSegmentMode_WS2812FX(uint8_t segment_index, uint8_t m);
void setCustomMode_WS2812FX(uint16_t (*p)());
void setSpeed_WS2812FX(uint16_t s);
void setReverse_WS2812FX(bool reverse);
void setSegmentReverse_WS2812FX(uint8_t segment_index, bool reverse);
bool getSegmentReverse_WS2812FX(uint8_t segment_index);
uint8_t getSegmentStart_WS2812FX(uint8_t segment_index);
void setSegmentStart_WS2812FX(uint8_t segment_index, uint8_t start_index);
uint8_t getSegmentStop_WS2812FX(uint8_t segment_index);
void setSegmentStop_WS2812FX(uint8_t segment_index, uint8_t stop);
const char* getSegmentModeString_WS2812FX(uint8_t segment_index);
uint16_t getSegmentSpeed_WS2812FX(uint8_t segment_index);
void setSegmentSpeed_WS2812FX(uint8_t segment_index, uint16_t segment_speed);
uint16_t getSegmentColor_WS2812FX(uint8_t segment_index, uint8_t color_index);
void increaseSpeed_WS2812FX(uint8_t s);
void decreaseSpeed_WS2812FX(uint8_t s);
void setColor_WS2812FX(uint8_t r, uint8_t g, uint8_t b);
void setArrayColor_WS2812FX(uint8_t r, uint8_t g, uint8_t b, uint8_t index);
void setColor_packed_WS2812FX(uint32_t c);
void setArrayColor_packed_WS2812FX(uint32_t c, uint8_t index);
void setSegmentArrayColor_packed_WS2812FX(uint8_t segment_index,
                                          uint8_t color_index, uint32_t c);
void setBrightness_WS2812FX(uint8_t b);
void increaseBrightness_WS2812FX(uint8_t s);
void decreaseBrightness_WS2812FX(uint8_t s);
void trigger_WS2812FX(void);
void setNumSegments_WS2812FX(uint8_t n);
void setSegment_WS2812FX(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode,
                         uint32_t color, uint16_t speed, bool reverse);
void setSegment_color_array_WS2812FX(uint8_t n, uint16_t start, uint16_t stop,
                                     uint8_t mode, const uint32_t colors[],
                                     uint16_t speed, bool reverse);
void resetSegments_WS2812FX();
void moveSegment_WS2812FX(uint8_t src, uint8_t dest);

bool isRunning_WS2812FX(void);

uint8_t getMode_WS2812FX(void);
uint8_t getBrightness_WS2812FX(void);
uint8_t getModeCount_WS2812FX(void);
uint8_t getNumSegments_WS2812FX(void);

uint16_t getSpeed_WS2812FX(void);
bool getReverse_WS2812FX(void);
uint16_t getLength_WS2812FX(void);
uint32_t getColor_WS2812FX(void);
uint32_t getArrayColor_WS2812FX(uint8_t index);

const char *getModeName_WS2812FX(uint8_t m);

void setCustomMode(uint16_t (*p)());

#endif
