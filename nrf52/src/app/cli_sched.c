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

#include "cli_sched.h"
#include "cli.h"
#include "conf_sched.h"
#include "conf_sched.inc"

static int is_between(int h, int m, int start_h, int start_m, int end_h,
                      int end_m)
{
    int flat_start = 100 * start_h + start_m;
    int flat_end = 100 * end_h + end_m;
    int flat_time = 100 * h + m;

    return flat_time >= flat_start && flat_time < flat_end;
}

static int parse_time(const char *time, unsigned int *p_h, unsigned int *p_m)
{
    if (sscanf(time, "%u:%u", p_h, p_m) != 2) {
        return 0;
    }

    if (time[strlen(time) - 3] != ':') {
        return 0;
    }

    if (*p_h > 23 || *p_m > 59) {
        return 0;
    }

    return 1;
}

static const char *skip_spaces(const char *p)
{
    while (*p != '\0' && *p == ' ') {
        p++;
    }

    return p;
}

static const char *skip_word(const char *p)
{
    while (*p != '\0' && *p != ' ') {
        p++;
    }

    return p;
}

static void print_from_to(nrf_cli_t const *p_cli, const char *from,
                          const char *to)
{
    while (from != to) {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "%c", *from);
        from++;
    }
}

/* Print entire words, up to n characters.  Return the point in the string
   where we stopped.  */

static const char *print_up_to_n_chars(nrf_cli_t const *p_cli, const char *text,
                                       const int n)
{
    int total = 0;

    text = skip_spaces(text);

    while (*text) {
        const char *word_end = text + 1;

        word_end = skip_word(word_end);

        if (total + (word_end - text) <= n) {
            print_from_to(p_cli, text, word_end);

            total += (word_end - text);

            text = word_end;
        } else {
            return text;
        }
    }

    return NULL;
}

static void print_day_help(nrf_cli_t const *p_cli, const char *day)
{
    static const nrf_cli_getopt_option_t opt[] = {
        NRF_CLI_OPT(NULL, "-v",
                    "Be more verbose. Use once to show abstracts, twice to "
                    "show detailed description.")};
    nrf_cli_help_print(p_cli, opt, ARRAY_SIZE(opt));
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                    "\r\n"
                    "You can optionally specify a time, to only show talks or "
                    "workshops happening at\r\n"
                    "that time.  For example:\r\n"
                    "\r\n"
                    "    schedule %s 12:30\r\n",
                    day);
}

static void do_schedule_day_common(nrf_cli_t const *p_cli, int argc,
                                   char **argv, const struct talk *talks,
                                   int ntalks, const char *day)
{
    int time_specified = 0;
    unsigned int h, m;
    int verbosity = 0;
    const char *time_str = NULL;

    if (nrf_cli_help_requested(p_cli)) {
        print_day_help(p_cli, day);
        return;
    }

    for (int n = 1; n < argc; n++) {
        if (strcmp(argv[n], "-v") == 0) {
            verbosity++;
        } else {
            if (time_specified) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "Too many arguments.\r\n");
                return;
            }
            if (!parse_time(argv[n], &h, &m)) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "%s is not a valid time\r\n", argv[n]);
                return;
            }

            time_str = argv[n];
            time_specified = 1;
        }
    }

    int n_displayed = 0;
    for (int i = 0; i < ntalks; i++) {
        const struct talk *t = &talks[i];

        if (!time_specified ||
            is_between(h, m, t->start_h, t->start_m, t->end_h, t->end_m)) {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "- ");
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_WHITE,
                            "\x1b[4m\x1b[1m%s\x1b[m\r\n", t->title);
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                            "  presented by %s\r\n", t->names);
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                            "  %u:%02u to %u:%02u, room: %s\r\n", t->start_h,
                            t->start_m, t->end_h, t->end_m, tracks[t->track]);

            if (verbosity > 0) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "  \x1b[4mAbstract\x1b[m: ");
                const char *text = t->abstract;

                text = print_up_to_n_chars(p_cli, text, 68);
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");

                while (text) {
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "  ");
                    text = print_up_to_n_chars(p_cli, text, 78);
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");
                }
            }
            if (verbosity > 1) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "  \x1b[4mDescription\x1b[m: ");
                const char *text = t->detailed;

                text = print_up_to_n_chars(p_cli, text, 65);
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");

                while (text) {
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "  ");
                    text = print_up_to_n_chars(p_cli, text, 78);
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                    " \r\n");
                }
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");

            n_displayed++;
        }
    }

    if (time_specified && n_displayed == 0) {
        printf("No talks are happening at %s\r\n", time_str);
    }
}

static void do_schedule_thursday(nrf_cli_t const *p_cli, size_t argc,
                                 char **argv)
{
    do_schedule_day_common(p_cli, argc, argv, talks_16,
                           sizeof(talks_16) / sizeof(talks_16[0]), "thursday");
}

static void do_schedule_friday(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
    do_schedule_day_common(p_cli, argc, argv, talks_17,
                           sizeof(talks_17) / sizeof(talks_17[0]), "friday");
}

static void do_schedule_speakers(nrf_cli_t const *p_cli, size_t argc,
                                 char **argv)
{
    int nspeakers = ARRAY_SIZE(speakers);
    int verbosity = 0;
    int n_displayed = 0;
    const char *filter = NULL;

    if (nrf_cli_help_requested(p_cli)) {
        static const nrf_cli_getopt_option_t opt[] = {
            NRF_CLI_OPT(NULL, "-v", "Be more verbose, show speakers' bio.")};
        nrf_cli_help_print(p_cli, opt, ARRAY_SIZE(opt));
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                        "\r\nYou can optionally specify a string to filter "
                        "down by speaker name.\r\n");
        return;
    }

    for (int n = 1; n < argc; n++) {
        if (strcmp(argv[n], "-v") == 0) {
            verbosity++;
        } else {
            if (filter) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "Too many arguments.\r\n");
                return;
            }

            filter = argv[n];
        }
    }

    for (int i = 0; i < nspeakers; i++) {
        const struct speaker *s = &speakers[i];

        if (!filter || strstr(s->name, filter)) {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "- ");
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_WHITE,
                            "\x1b[4m\x1b[1m%s\x1b[m\r\n", s->name);

            if (verbosity > 0) {
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                                "  \x1b[4mBio\x1b[m: ");
                const char *text = s->bio;

                text = print_up_to_n_chars(p_cli, text, 73);
                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");

                while (text) {
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "  ");
                    text = print_up_to_n_chars(p_cli, text, 78);
                    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");
                }

                nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\r\n");
            }

            n_displayed++;
        }
    }

    if (filter && n_displayed == 0) {
        printf("No speakers match %s\r\n", filter);
    }
}

static void do_schedule(nrf_cli_t const *p_cli, size_t argc, char **argv)
{
    nrf_cli_help_print(p_cli, NULL, 0);
}

#if defined(NSEC_FLAVOR_CONF)
NRF_CLI_CREATE_STATIC_SUBCMD_SET(schedule_sub){
    NRF_CLI_CMD(thursday, NULL, "Display Thursday schedule",
                do_schedule_thursday),
    NRF_CLI_CMD(friday, NULL, "Display Friday schedule", do_schedule_friday),
    NRF_CLI_CMD(speakers, NULL, "Display list of speakers",
                do_schedule_speakers),
    NRF_CLI_SUBCMD_SET_END};

NRF_CLI_CMD_REGISTER(schedule, &schedule_sub, CMD_SCHEDULE_HELP, do_schedule);
#endif
