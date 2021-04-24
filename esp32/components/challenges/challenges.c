#include "argtable3/argtable3.h"
#include "challenge1.h"
#include "challenge2.h"
#include "esp_console.h"
#include "esp_log.h"
#include <string.h>

static struct {
    struct arg_str *type;
    struct arg_str *code;
    struct arg_end *end;
} challenges_args;

static int challenges(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&challenges_args);
    char challenge1[5] = "RE101";
    char challenge2[5] = "RE102";

    if (nerrors != 0) {
        arg_print_errors(stderr, challenges_args.end, argv[0]);
        return 1;
    }

    if (!strncmp(challenge1, challenges_args.type->sval[0], 5)) {
        printf("checking RE101\n");
        ESP_LOGI(__func__, "Submitting code for %s",
                 challenges_args.type->sval[0]);

        challenge_RE101(challenges_args.code->sval[0]);
    } else if (!strncmp(challenge2, challenges_args.type->sval[0], 5)) {
        printf("checking RE102\n");
        ESP_LOGI(__func__, "Submitting code for %s",
                 challenges_args.type->sval[0]);

        challenge_RE102(challenges_args.code->sval[0]);
    } else {
        ESP_LOGW(__func__, "Type unknown");
    }

    return 0;
}

void register_challenges(void)
{
    challenges_args.type = arg_str0(NULL, NULL, "<RE(101|102)>", NULL);
    challenges_args.code = arg_str1(NULL, NULL, "<code>", NULL);
    challenges_args.end = arg_end(2);

    const esp_console_cmd_t challenges_cmd = {.command = "challenges",
                                              .help = "???",
                                              .hint = NULL,
                                              .func = &challenges,
                                              .argtable = &challenges_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&challenges_cmd));
}
