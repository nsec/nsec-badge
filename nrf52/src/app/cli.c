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

bool standard_check(const nrf_cli_t *p_cli, size_t argc,
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
