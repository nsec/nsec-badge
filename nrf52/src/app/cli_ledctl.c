/*
 * Copyright 2019 Eric Tremblay <habscup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "cli.h"

#include <drivers/cli_uart.h>
#include "drivers/display.h"
#include "drivers/ws2812fx.h"

#include "nsec_led_pattern.h"
#include "persistency.h"

static void do_led_create(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index, start_index, stop_index;

    if (!standard_check(p_cli, argc, 3, argv, NULL, 0)) {
        return;
    }

    segment_index = getNumSegments_WS2812FX();
    if (segment_index >= MAX_NUM_SEGMENTS) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "Can't create more segment (limit = 15) \r\n");
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= NEOPIXEL_COUNT || val < 0) {
        nrf_cli_fprintf(
            p_cli, NRF_CLI_ERROR,
            "%s: Invalid parameter, start need to be 0 <= start <= %d\r\n",
            argv[1], NEOPIXEL_COUNT - 1);
        return;
    } else {
        start_index = val;
    }


    val = strtol(argv[2], NULL, 10);
    if (val == 0 && strcmp(argv[2], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[2]);
        return;
    } else if (val >= NEOPIXEL_COUNT || val < 0) {
        nrf_cli_fprintf(
            p_cli, NRF_CLI_ERROR,
            "%s: Invalid parameter, stop need to be 0 <= start <= %d\r\n",
            argv[1], NEOPIXEL_COUNT - 1);
        return;
    } else if (val < start_index) {
        nrf_cli_fprintf(
            p_cli, NRF_CLI_ERROR,
            "%s: Invalid parameter, stop need to be <= start\r\n", argv[2]);
        return;
    } else {
        stop_index = val;
    }

    setSegment_WS2812FX(segment_index, start_index, stop_index, FX_MODE_BLINK,
                         BLUE, DEFAULT_SPEED, false);

    update_stored_num_segment(segment_index + 1, false);
    update_stored_segment(segment_index, start_index, stop_index, FX_MODE_BLINK,
                          BLUE, 0, 0, DEFAULT_SPEED, false, true);

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "New segment created! You can now configure your segment "
                    "with the other commands. Please note that segment can and "
                    "will overlap with eachother.\r\n");
}

static void do_led_delete(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    uint8_t segment_count = getNumSegments_WS2812FX();

    if (segment_count == 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "Can't delete the last segment\r\n");
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val == 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Can't delete segment 0\r\n");
        return;
    } else if (val >= segment_count || val < 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Segment does not exist\r\n");
        return;
    } else {
        segment_index = val;
    }

    uint16_t segment_stop = getSegmentStop_WS2812FX(segment_index);
    setSegmentStop_WS2812FX(segment_index - 1, segment_stop);
    update_stored_stop(segment_index - 1, segment_stop, false);

    setNumSegments_WS2812FX(segment_count - 1);
    update_stored_num_segment(segment_count - 1, false);


    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Segment %d was deleted.\r\n",
                    segment_index);

    if (segment_index == segment_count - 1) {
        return;
    }

    for (int i = segment_count - 1; i != segment_index; i--) {
        moveSegment_WS2812FX(i, i-1);
        move_stored_segment(i, i-1, false);
    }

    update_persistency();
}

static void do_led_show(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Configured segment:\r\n");
    for (int i = 0; i < getNumSegments_WS2812FX(); i++) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
            "**********************************************************\r\n");
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Index: %d\r\n", i);
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Start: %d\r\n",
                        getSegmentStart_WS2812FX(i));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Stop: %d\r\n",
                        getSegmentStop_WS2812FX(i));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Mode: %s\r\n",
                        getSegmentModeString_WS2812FX(i));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Speed: %d\r\n",
                        getSegmentSpeed_WS2812FX(i));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Reverse?: %s\r\n",
                        getSegmentReverse_WS2812FX(i) ? "true" : "false");
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "First color: %06X\r\n",
                        getSegmentColor_WS2812FX(i, 0));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Second color: %06X\r\n",
                        getSegmentColor_WS2812FX(i, 1));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Third color: %06X\r\n",
                        getSegmentColor_WS2812FX(i, 2));
    }
}

static void do_led_reset(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    static bool called = false;

    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    if (argc == 2 && !strcmp("cancel", argv[1])) {
        called = false;
        return;
    } else if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (!called) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "Are you sure you want to reset segment configuration "
                        "? If yes, call the command again. If no, call it with "
                        "the option 'cancel' \r\n");
        called = true;
    } else {
        resetSegments_WS2812FX();
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "Segments restored to factory default\r\n");
        called = false;
    }

    load_stored_led_default_settings();
}

static void do_led_segment(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                    argv[0], argv[1]);
}

static void do_index_select(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index = 0;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX() && val >= 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "%s: index must be between 0 and %d\r\n",
                        argv[1], getNumSegments_WS2812FX() - 1);
        return;
    } else {
        segment_index = val;
    }

    if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "First: %d, Last: %d\r\n",
                        getSegmentStart_WS2812FX(segment_index),
                        getSegmentStop_WS2812FX(segment_index));
        return;
    } else if (argc == 4) {
        uint8_t start_index, stop_index;
        val = strtol(argv[2], NULL, 10);
        if (val == 0 && strcmp(argv[2], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
            return;
        } else if (val < 0) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Invalid parameter must be positive\r\n",
                            argv[1]);
            return;
        }
        start_index = val;

        val = strtol(argv[3], NULL, 10);
        if (val == 0 && strcmp(argv[3], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                            argv[1]);
            return;
        } else if (val < start_index) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "Last LED must be >= First LED\r\n");
            return;
        } else if (val < 0) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Invalid parameter must be positive\r\n",
                            argv[1]);
            return;
        }
        stop_index = val;

        setSegmentStart_WS2812FX(segment_index, start_index);
        update_stored_start(segment_index, start_index, false);

        setSegmentStop_WS2812FX(segment_index, stop_index);
        update_stored_stop(segment_index, stop_index, true);

        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
            "Segment %d, now start at %d and finish at %d\r\n", segment_index,
            start_index, stop_index);
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
}

static void do_mode_list(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Available pattern: \r\n");

    for (int i = 0; i < MODE_COUNT - 1; i++) {
        uint8_t index_ps = get_extra_array_index(i);
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%d: %s %s\r\n", i,
            getModeName_WS2812FX(i),
            pattern_is_unlock(index_ps) ? "<Locked>" : "");
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "Go see sponsors and enter password on your badge to "
                    "unlock locked passwords :)\r\n");
}

static void do_led_mode(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index = 0;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    if (!strcmp("list", argv[1])) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX() || val < 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "%s: index must be between 0 and %d\r\n",
                        argv[1], getNumSegments_WS2812FX() - 1);
        return;
    } else {
        segment_index = val;
    }

    if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%s\r\n",
                    getSegmentModeString_WS2812FX(segment_index));
        return;
    } else if (argc == 3) {
        val = strtol(argv[2], NULL, 10);
        if (val == 0 && strcmp(argv[2], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
            return;
        } else if (val >= MODE_COUNT || val < 0) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Index must be between 0 and %d\r\n", argv[2],
                            MODE_COUNT - 1);
            return;
        } else if (pattern_is_unlock(val)) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Pattern is locked, go see sponsor and enter "
                            "the password in the badge :)\r\n",
                            argv[2]);
            return;
        } else {
            setSegmentMode_WS2812FX(segment_index, val);
            update_stored_mode(segment_index, val, true);
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                            "Segment %d, now use %s pattern\r\n", segment_index,
                            getSegmentModeString_WS2812FX(segment_index));
        }
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                        "set", argv[2]);
        return;
    }
}

static void do_led_color(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index = 0;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX() || val < 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "%s: index must be between 0 and %d\r\n",
                        argv[1], getNumSegments_WS2812FX() - 1);
        return;
    } else {
        segment_index = val;
    }

    if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "1: %d(%02X)\r\n2: %d(%02X)\r\n3: %d(%02X)\r\n",
                        getSegmentColor_WS2812FX(segment_index, 0),
                        getSegmentColor_WS2812FX(segment_index, 0),
                        getSegmentColor_WS2812FX(segment_index, 1),
                        getSegmentColor_WS2812FX(segment_index, 1),
                        getSegmentColor_WS2812FX(segment_index, 2),
                        getSegmentColor_WS2812FX(segment_index, 2));
        return;
    } else if (argc == 4) {
        uint8_t color_index;
        uint16_t color;
        val = strtol(argv[2], NULL, 10);
        if (val == 0 && strcmp(argv[2], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
            return;
        } else if (val >= 3 || val < 0) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Index must be between 0 and 2\r\n", argv[2]);
            return;
        }
        color_index = val;

        val = strtol(argv[3], NULL, 10);
        if (val == 0 && strcmp(argv[3], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
            return;
        } else if (val >= 0xFFFFFF || val < 0) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: Index must be between 0 and 16777215\r\n",
                            argv[3]);
            return;
        }
        color = val;

        setSegmentArrayColor_packed_WS2812FX(segment_index, color_index, color);
        update_stored_color(segment_index, color_index, color, true);
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
            "Segment %d, color %d is now: %d(%02X)\r\n", segment_index,
            color_index, color, color);
    } else if (argc == 5) {
        uint16_t color[3];
        for (int i = 0; i < 3; i++) {
            val = strtol(argv[i + 2], NULL, 10);
            if (val == 0 && strcmp(argv[i + 2], "0")) {
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                                "%s: Invalid parameter\r\n", argv[i + 2]);
                return;
            } else if (val >= 0xFFFFFF || val < 0) {
                nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                                "%s: Index must be between 0 and 16777215\r\n",
                                argv[i + 2]);

                return;
            }
            color[i] = val;
        }

        setSegmentArrayColor_packed_WS2812FX(segment_index, 0, color[0]);
        setSegmentArrayColor_packed_WS2812FX(segment_index, 1, color[1]);
        setSegmentArrayColor_packed_WS2812FX(segment_index, 2, color[2]);
        update_stored_color(segment_index, 0, color[0], false);
        update_stored_color(segment_index, 1, color[1], false);
        update_stored_color(segment_index, 2, color[2], true);

        nrf_cli_fprintf(
            p_cli, NRF_CLI_DEFAULT,
            "Segment %d color are now: \n\r1: %d(%02X)\r\n2: %d(%02X)\r\n3: "
            "%d(%02X)\r\n", segment_index,
            getSegmentColor_WS2812FX(segment_index, 0),
            getSegmentColor_WS2812FX(segment_index, 0),
            getSegmentColor_WS2812FX(segment_index, 1),
            getSegmentColor_WS2812FX(segment_index, 1),
            getSegmentColor_WS2812FX(segment_index, 2),
            getSegmentColor_WS2812FX(segment_index, 2));
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                        "set", argv[2]);
        return;
    }
}

static void do_led_speed(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index = 0;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX() || val < 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "%s: index must be between 0 and %d\r\n",
                        argv[1], getNumSegments_WS2812FX() - 1);
        return;
    } else {
        segment_index = val;
    }

    if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%d\r\n",
                    getSegmentSpeed_WS2812FX(segment_index));
        return;
    } else if (argc == 3) {
        val = strtol(argv[2], NULL, 10);
        if (val == 0 && strcmp(argv[2], "0")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                            argv[2]);
            return;
        } else if (val < 0 || val > 5000) {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                            "%s: speed must be between 0 and 5000\r\n",
                            argv[2]);
            return;
        }

        setSegmentSpeed_WS2812FX(segment_index, val);
        update_stored_speed(segment_index, val, true);
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Segment %d speed is now %d\r\n",
                    segment_index, getSegmentSpeed_WS2812FX(segment_index));
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                        "set", argv[2]);
        return;
    }
}

static void do_led_brightness(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    if (argc == 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%d\r\n",
                        getBrightness_WS2812FX());
        return;
    }

    if (argc != 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= 0 && val <= 100) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                        "Set leds brightness to: %ld\r\n", val);
        setBrightness_WS2812FX(val);
        update_stored_brightness(val, true);
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Value out of range\r\n",
                        argv[2]);
        nrf_cli_help_print(p_cli, NULL, 0);
    }
}

static void do_led_reverse(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index = 0;

    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX() || val < 0) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR,
                        "%s: index must be between 0 and %d\r\n",
                        argv[1], getNumSegments_WS2812FX() - 1);
        return;
    } else {
        segment_index = val;
    }

    if (argc == 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%s\r\n",
                    getSegmentReverse_WS2812FX(segment_index) ? "reverse"
                                                              : "normal");
        return;
    } else if (argc == 3) {
        if (!strcmp(argv[2], "normal")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                            "Set led execution direction to: %s\r\n",
                            argv[2]);
            setSegmentReverse_WS2812FX(segment_index, false);
            update_stored_reverse(segment_index, false, true);
            return;
        } else if (!strcmp(argv[2], "reverse")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                            "Set led execution direction to: %s\r\n",
                            argv[2]);
            setSegmentReverse_WS2812FX(segment_index, true);
            update_stored_reverse(segment_index, true, true);
            return;
        } else {
            nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                            "set", argv[2]);
            return;
        }
    }
}

static void do_led(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                    argv[0], argv[1]);
}

#define create_help                                                            \
    "Create a new segment of leds \r\n Usage: ledctl segment create "          \
    "{start_index} {stop_index}"

#define delete_help                                                            \
    "Delete a segment of leds and combine it with the previous one.\r\n "      \
    "Usage: ledctl segment delete {segment_index}"

#define reset_help                                                             \
    "Reset segments configuration to factory default a confirmation will be "  \
    "asked.\r\n Usage: ledctl segment reset {cancel}"

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_segment) {
    NRF_CLI_CMD(create, NULL, create_help, do_led_create),
    NRF_CLI_CMD(delete, NULL, delete_help, do_led_delete),
    NRF_CLI_CMD(show, NULL, "Show all configured segment", do_led_show),
    NRF_CLI_CMD(reset, NULL, reset_help, do_led_reset),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_mode) {
    NRF_CLI_CMD(list, NULL, "List all available mode with index", do_mode_list),
    NRF_CLI_SUBCMD_SET_END
};

#define index_select_help                                                      \
    "Get or set first and last led of a segment\r\n"                           \
    "Usage:\r\n"                                                               \
    "Set: ledctl index_select {segment index} {First led [0-14]} "             \
    "{Last led [1-14]}\r\n"                                                    \
    "Get: ledctl index_select {segment_index}\r\n"

#define reverse_help                                                           \
    "Get or set mode execution direction\r\n"                                  \
    "Usage:\r\n"                                                               \
    "Set: ledctl reverse {segment_index} {normal|reverse}\r\n"                 \
    "Get: ledctl reverse {segment_index}\r\n"

#define brightness_help                                                        \
    "Get or set the leds brightness\r\n"                                       \
    "Usage:\r\n"                                                               \
    "Set: ledctl brightness {0-100}\r\n"                                       \
    "Get: ledctl brightness\r\n"                                               \

#define mode_help                                                              \
    "Get or set the leds mode for a segment\r\n"                               \
    "Usage:\r\n"                                                               \
    "Set: ledctl mode {segment_index} {0-56}\r\n"                              \
    "Get: ledctl mode {segment_index}\r\n"                                     \
    "list: ledctl mode list\r\n"                                               \

#define color_help                                                             \
    "Get or set three primary color for the segments\r\n"                      \
    "Colors are expressed in RGB format\r\n"                                   \
    "The segment are using three colors to build there pattern\r\n"            \
    "Usage:\r\n"                                                               \
    "Set all: ledctl color {segment_index} {0-16777215} {0-16777215} {0-16777215}\r\n"   \
    "Set by index: ledctl color {segment_index} {0-2} {0-16777215}\r\n"            \
    "Get: ledctl mode {segment_index}\r\n"                                     \

#define speed_help                                                             \
    "Get or set mode speed\r\n"                                                \
    "Usage:\r\n"                                                               \
    "Set: ledctl speed {segment_index} {0-5000}\r\n"                           \
    "Get: ledctl speed {segment_index}\r\n"

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_led){
    NRF_CLI_CMD(segment, &sub_segment, "Manage LED segments", do_led_segment),
    NRF_CLI_CMD(index_select, NULL, index_select_help, do_index_select),
    NRF_CLI_CMD(mode, &sub_mode, mode_help, do_led_mode),
    NRF_CLI_CMD(color, NULL, color_help, do_led_color),
    NRF_CLI_CMD(speed, NULL, speed_help, do_led_speed),
    NRF_CLI_CMD(brightness, NULL, brightness_help, do_led_brightness),
    NRF_CLI_CMD(reverse, NULL, reverse_help, do_led_reverse),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(ledctl, &sub_led, "Control LEDs configuration", do_led);
