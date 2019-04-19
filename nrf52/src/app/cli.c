/*
 * Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
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
#include "drivers/nsec_storage.h"

static bool standard_check(const nrf_cli_t *p_cli, size_t argc,
                           size_t minimum_arg, char **argv,
                           nrf_cli_getopt_option_t const *p_opt)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, p_opt, ARRAY_SIZE(p_opt));
        return false;
    }

    if (argc < minimum_arg) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, p_opt, ARRAY_SIZE(p_opt));
        return false;
    }

    return true;
}

static struct {
    const char *n_line, *sec_line;
} nsec_rows[] = {
    // clang-format off
    {"                       ", "                                        `/ydmmmdy+.      "},
    {"                       ", "                                       /mNNmhyhmNNNy`    "},
    {"                       ", "                                      :NNN+`    -dNNh    "},
    {"     -ohy  `:+oo+/.    ", "  -+yhdmmdhyo/.      .:/+++/:.        /NNd-:/+++/oNNN`   "},
    {"  :hNMMMh-hMMMMMMMMNo  ", ":dNNNNNNNNNNNNNd. -sdNNNNNNNNNds-     /NNNNNNNNNNNNNm-   "},
    {"  sMMMMMNMMMMMMMMMMMMd ", "hNNNNmyssydNNNm/.yNNNNNNNNNNNNNNNy` `sNNNNNNNNNNNNNNNm/  "},
    {"  sMMMMMMMy+/+yNMMMMMM+", " NNNN/     `:o:-mNNNNmo-` `-sNNNNNd:mNNNNNdo:--:+yNms`   "},
    {"  sMMMMMN.     .NMMMMMy", " mNNNNho/-`   `mNNNNm-       /NNNNNNNNNNNo        ..     "},
    {"  sMMMMMh       hMMMMMy", " mNNNNNNNNNds/+NNNNNdsssssssssNNNNNNNNNNh                "},
    {"  sMMMMMh       hMMMMMy", " -odNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNy                "},
    {"  sMMMMMh       hMMMMMy", "    `-/oydNNNNNNNNNNm:::::::::::::/NNNNNm.               "},
    {"  sMMMMMh       hMMMMMy", " o:`      .mNNNNNNNNNy.       -o-  sNNNNNm+`     .oy:    "},
    {"  sMMMMMh       hMMMMMy", " mNmhs+//+yNNNNNdNNNNNNhsoosymNNNh: omNNNNNNdhyhmNNNNh:  "},
    {"  sMMMMMh       hMMMMMy", " mNNNNNNNNNNNNNy`-ymNNNNNNNNNNNNmy-  .smNNNNNNNNNNNNms.  "},
    {"  +hhhhho       ohhhhho", " /shmNNNNNNmho-    `:oyhdmmmdyo:`      `:oyhdmmddyo:`    "},
    {"                       ", "      `..``                                              "},
    // clang-format on
};

static void do_nsec(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
#ifdef NSEC_FLAVOR_CTF
    if (nrf_cli_help_requested(p_cli)) {
        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "It looks like you could use some help, here's a flag!\r\n\r\n");
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_YELLOW,
                        "      FLAG-173f42fc-ade7-4d5e-ac0a-915f180fcb2b\r\n");
        return;
    }
#endif

    for (int i = 0; i < ARRAY_SIZE(nsec_rows); i++) {
        nrf_fprintf(p_cli->p_fprintf_ctx, "\x1b[38;2;19;118;188m");
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s", nsec_rows[i].n_line);
        nrf_fprintf(p_cli->p_fprintf_ctx, "\x1b[38;2;255;255;255m");
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s\r\n", nsec_rows[i].sec_line);
    }
}

NRF_CLI_CMD_REGISTER(nsec, NULL, "Welcome to NSEC!", do_nsec);

static void do_display(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        return;
    }

    if (strcmp(argv[1], "brightness")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                        argv[0], argv[1]);
    }
}

static void do_display_brightness(const nrf_cli_t *p_cli, size_t argc,
                                  char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        return;
    }

    if (strcmp(argv[1], "get") || strcmp(argv[1], "set")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                        argv[0], argv[1]);
    }
}

static void do_display_brightness_get(const nrf_cli_t *p_cli, size_t argc,
                                      char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "%d\r\n",
                    get_stored_display_brightness());
}

static void do_display_brightness_set(const nrf_cli_t *p_cli, size_t argc,
                                      char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL)) {
        return;
    }

    long int val = strtol(argv[1], NULL, 10);

    // strtol will return 0 on bad conversion, lets check if we really want 0
    if (val == 0 && strcmp(argv[1], "0")) {
        val = -1;
    }

    if (val == 0) {
        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "Set display brightness to: %d, display will be turn off\r\n", val);
    } else if (val > 0 && val <= 100) {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                        "Set display brightness to: %d\r\n", val);
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Value out of range\r\n",
                        argv[1]);
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    display_set_brightness(val);
    update_stored_display_brightness(val);
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_brightness){
    NRF_CLI_CMD(get, NULL, "Get brightness value", do_display_brightness_get),
    NRF_CLI_CMD(set, NULL, "Set brightness value [0-100]",
                do_display_brightness_set),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_display){
    NRF_CLI_CMD(brightness, &sub_brightness,
                "Get or set the display brightness", do_display_brightness),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(display, &sub_display, "Display configuration",
                     do_display);

static void do_led_create(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index, start_index, stop_index;

    if (!standard_check(p_cli, argc, 3, argv, NULL)) {
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
    } else if (val >= NEOPIXEL_COUNT) {
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
    } else if (val >= NEOPIXEL_COUNT) {
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

    //TODO Persist

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "New segment created! You can now configure your segment "
                    "with the other commands. Please note that segment can and "
                    "will overlap with eachother.\r\n");
}

static void do_led_delete(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    uint8_t segment_index;

    if (!standard_check(p_cli, argc, 2, argv, NULL)) {
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
    } else if (val >= segment_count) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Segment does not exist\r\n");
        return;
    } else {
        segment_index = val;
    }

    setSegmentStop_WS2812FX(segment_index - 1,
                            getSegmentStop_WS2812FX(segment_index));

    setNumSegments_WS2812FX(segment_count - 1);

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Segment %d was deleted.\r\n",
                    segment_index);

    if (segment_index == segment_count - 1) {
        return;
    }

    for (int i = segment_count - 1; i != segment_index; i--) {
        moveSegment_WS2812FX(i, i-1);
    }

    //TODO persist
}

static void do_led_show(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL)) {
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
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "First color: %02X\r\n",
                        getSegmentColor_WS2812FX(i, 0));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Second color: %02X\r\n",
                        getSegmentColor_WS2812FX(i, 1));
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Third color: %02X\r\n",
                        getSegmentColor_WS2812FX(i, 2));
    }
}

static void do_led_reset(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    static bool called = false;

    if (!standard_check(p_cli, argc, 1, argv, NULL)) {
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

    // Todo persist
}

static void do_led_segment(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL)) {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n",
                    argv[0], argv[1]);
}

static void do_led_mode(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
}

static void do_led_color(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
}

static void do_led_speed(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
}

static void do_led_brightness(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    long int val;
    if (!standard_check(p_cli, argc, 1, argv, NULL)) {
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
                        "Set leds brightness to: %d\r\n", val);
        setBrightness_WS2812FX(val);
        update_stored_brightness(val);
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

    if (!standard_check(p_cli, argc, 2, argv, NULL)) {
        return;
    }

    val = strtol(argv[1], NULL, 10);
    if (val == 0 && strcmp(argv[1], "0")) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: Invalid parameter\r\n",
                        argv[1]);
        return;
    } else if (val >= getNumSegments_WS2812FX()) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: index is greater than total segment count: %d\r\n",
                        argv[1], getNumSegments_WS2812FX());
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
            update_stored_reverse(false);
            return;
        } else if (!strcmp(argv[2], "reverse")) {
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                            "Set led execution direction to: %s\r\n",
                            argv[2]);
            setSegmentReverse_WS2812FX(segment_index, true);
            update_stored_reverse(true);
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
    if (!standard_check(p_cli, argc, 2, argv, NULL)) {
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

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_segment){
    NRF_CLI_CMD(create, NULL, create_help, do_led_create),
    NRF_CLI_CMD(delete, NULL, delete_help, do_led_delete),
    NRF_CLI_CMD(show, NULL, "Show all configured segment", do_led_show),
    NRF_CLI_CMD(reset, NULL, reset_help, do_led_reset),
    NRF_CLI_SUBCMD_SET_END};

#define reverse_help                                                           \
    "Get or set mode execution direction\r\n Usage:\r\n Set: ledctl reverse "  \
    "{segment_index} {normal|reverse} \r\n Get: ledctl reverse "                \
    "{segment_index}"

#define brightness_help                                                        \
    "Get or set the leds brightness\r\n Usage:\r\n Set: ledctl brightness "    \
    "{0-100} \r\n Get: ledctl brightness"

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_led){
    NRF_CLI_CMD(segment, &sub_segment, "Manage LED segments", do_led_segment),
    NRF_CLI_CMD(mode, NULL, "Control LED mode", do_led_mode),
    NRF_CLI_CMD(color, NULL, "Control LED color array", do_led_color),
    NRF_CLI_CMD(speed, NULL, "Control LED mode execution speed", do_led_speed),
    NRF_CLI_CMD(brightness, NULL, brightness_help, do_led_brightness),
    NRF_CLI_CMD(reverse, NULL, reverse_help, do_led_reverse),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(ledctl, &sub_led, "Control LEDs configuration", do_led);

/* Initialize the command-line interface module.  */

void cli_init(void)
{
    cli_uart_init();
}

/* Function that needs to be called periodically to do CLI stuff (read
   characters in, potentially call handlers).  */

void cli_process(void)
{
    cli_uart_process();
}
