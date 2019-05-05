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

#include "cli_sched.h"
#include "drivers/display.h"
#include "persistency.h"
#include "random.h"
#include "cli_sched.h"
#include <drivers/cli_uart.h>

bool standard_check(const nrf_cli_t *p_cli, size_t argc, size_t minimum_arg,
                    char **argv, nrf_cli_getopt_option_t const *p_opt,
                    size_t opt_len)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (nrf_cli_help_requested(p_cli)) {
        nrf_cli_help_print(p_cli, p_opt, opt_len);
        return false;
    }

    if (argc < minimum_arg) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, p_opt, opt_len);
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

static void do_displayctl(const nrf_cli_t *p_cli, size_t argc, char **argv)
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

static void do_displayctl_brightness(const nrf_cli_t *p_cli, size_t argc,
                                  char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    if (argc == 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "%d\r\n",
                        get_stored_display_brightness());
        return;
    }

    if (argc != 2) {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        nrf_cli_help_print(p_cli, NULL, 0);
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

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_displayctl){
    NRF_CLI_CMD(brightness, NULL, "Get or set the display brightness",
                do_displayctl_brightness),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(displayctl, &sub_displayctl, "Display configuration",
                     do_displayctl);

static void do_identity(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    if (argc == 1) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Identity: %s\r\n", get_stored_identity());
        return;
    } else if (argc == 2) {
        char new_identity[17];
        snprintf(new_identity, 16, "%s", argv[1]);
        new_identity[16] = '\0';
        update_identity(new_identity);
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Identity: %s\r\n", get_stored_identity());
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n",
                        argv[0]);
        return;
    }
}

NRF_CLI_CMD_REGISTER(identity, NULL,
                         "Get or set the badge identity\r\nUsage:\r\nGet: "
                         "identity\r\nSet: identity "
                         "{new_identity}\r\nMaximum of 16 char",
                         do_identity);

#ifdef NSEC_FLAVOR_CTF
static void do_dump(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }
}

static void do_external(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 2, argv, NULL, 0)) {
        return;
    }
}

static void do_external_memory(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (!standard_check(p_cli, argc, 1, argv, NULL, 0)) {
        return;
    }

    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT,
        "Offset(h)\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");

    for (int i = 0; i < 256; i++) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%08X\t", i * 16);
        for (int j = 0; j < 16; j++) {
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%02X ", nsec_random_get_byte(255));
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "\r\n");
    }

    nrf_cli_fprintf(
        p_cli, NRF_CLI_DEFAULT,
        "%08X\t46 4C 41 47 2D 4E 30 74 54 68 33 4D 33 6D 30 72 \r\n%08X\t79 59 "
        "30 75 41 72 33 4C 30 30 6B 31 6E 67 46 30 \r\n%08X\t72 00 FF 09 "
        "A4 B7 E1 D1 F0 F5 22 43 1A 63 52 42 \r\n",
        256 * 16, 257 * 16, 258 * 16);

    for (int i = 259; i < 512; i++) {
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%08X\t", i * 16);
        for (int j = 0; j < 16; j++) {
            nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "%02X ", nsec_random_get_byte(255));
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "\r\n");
    }
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_external){
    NRF_CLI_CMD(memory, NULL, "Dump external memory!", do_external_memory),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sub_dump){
    NRF_CLI_CMD(external, &sub_external, "Dump external stuff...", do_external),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(dump, &sub_dump, "Dump things...", do_dump);

static void do_ask_buddha(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
	const char *buddha_flag = "FLAG-A_THOUSAND_CHAR_JOURNEY_BEGINS";
	size_t idx = 0;
    if (nrf_cli_help_requested(p_cli)) {

        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_RED,
            "TIME");
        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            " IS LIKE A RIVER.\r\n");
        return;
    }

	int cy = 1<<31;
	while (buddha_flag[idx] == argv[1][idx] && buddha_flag[idx] != 0) {
		for (uint32_t jdx = 0; jdx < 0x003FFFFF; jdx++) {
			__asm__ __volatile__ ("nop");
		}
		idx += 1;
	}
	if (buddha_flag[idx] == argv[1][idx] && buddha_flag[idx] == 0) {
		nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Now go ask god.\r\n");
	}
	return;

}

NRF_CLI_CMD_REGISTER(askbuddha, NULL, "Buddha only knows one flag.", do_ask_buddha);

static void do_ask_jeez(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli)) {

        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "AND SURELY I AM WITH YOU ALWAYS TO THE VERY END OF THE AGE.\r\n");
        nrf_cli_fprintf(
            p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "\tMATTHEW 28:20\r\n");
        return;
    }

	if (strcmp("FLAG-TIMING_ATTTACKS_ARE_FUN", argv[1]) == 0) {
		nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "Now go ask god.\r\n");
	}
	return;

}

NRF_CLI_CMD_REGISTER(askjeez, NULL, "Jesus only knows one flag.", do_ask_jeez);

#endif

static void do_help(const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "Welcome to the NorthSec 2019 CLI interface!\r\n\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "blectl:     Utility to do some operation over BLE\r\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "displayctl: Adjust the brightness of the screen\r\n");

#ifdef NSEC_FLAVOR_CTF
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "askjeez:    In the christian lore, jesus is th son of god.\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "askbuddha:  Try asking for help.\r\n");
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "dump:       Dump external flash memory\r\n");
#endif

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "identity:   Change your badge name!\r\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "ledctl:     Utility to control the leds and create custom "
                    "flashing pattern\r\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "nsec:       Print a pretty NorthSec logo!\r\n");

#if defined(NSEC_FLAVOR_CONF)
    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "schedule:   " CMD_SCHEDULE_HELP "\r\n");
#endif

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT, "\r\n");

    nrf_cli_fprintf(p_cli, NRF_CLI_DEFAULT,
                    "All commands have a specific help menu which you can "
                    "access with -h/--help.\r\n");
}


NRF_CLI_CMD_REGISTER(help, NULL, "Print CLI help", do_help);
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
