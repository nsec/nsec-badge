#include "cmd_hidden.h"

#include "argtable3/argtable3.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#include "cmd_decl.h"
#include "esp_console.h"
#include "save.h"

static int hint(int argc, char **argv)
{
    printf("The legend says there is a hidden command, can you find it?\n");
    return 0;
}

void register_hint(void)
{
    const esp_console_cmd_t hint_cmd = {.command = "???",
                                        .help = "Don't you dare executing me!",
                                        .hint = NULL,
                                        .func = &hint,
                                        .argtable = NULL};

    ESP_ERROR_CHECK(esp_console_cmd_register(&hint_cmd));
}

static int hidden(int argc, char **argv)
{
    char flag[] = "RXUS9Wx%R$fRaz2Df$r%`";
    for (uint8_t i = 0; i < strlen(flag); i++) {
        flag[i] = flag[i] ^ 0x14;
    }
    printf("%s\n", flag);
    Save::save_data.flag7 = true;
    Save::write_save();

    return 0;
}

void register_hidden(void)
{
    static char cmd_hidden[] = "kwz@lhpm{@py@~ezmpkw>";
    for (uint8_t i = 0; i < strlen(cmd_hidden); i++) {
        cmd_hidden[i] = cmd_hidden[i] ^ 0x1f;
    }

    const esp_console_cmd_t hidden_cmd = {.command = cmd_hidden,
                                          .help = NULL,
                                          .hint = NULL,
                                          .func = &hidden,
                                          .argtable = NULL};

    ESP_ERROR_CHECK(esp_console_cmd_register(&hidden_cmd));
}
