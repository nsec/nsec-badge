#include "cmd_hidden.h"

#include "argtable3/argtable3.h"
#include "esp_log.h"

#include "cmd_decl.h"
#include "esp_console.h"
#include "save.h"

#include <iostream>
#include <string.h>

static int hint(int argc, char **argv)
{
    std::cout << "The legend says there is a hidden command, can you find it?"
              << std::endl;
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
    char message[] = {
        101, 189, 213, 201, 128, 141, 213, 201, 165, 189, 205, 165, 209, 229,
        128, 161, 133, 205, 128, 177, 149, 145, 128, 229, 189, 213, 128, 209,
        189, 128, 133, 128, 177, 149, 157, 149, 185, 145, 133, 201, 229, 128,
        209, 201, 149, 133, 205, 213, 201, 149, 128, 180, 128, 229, 189, 213,
        128, 133, 201, 149, 128, 209, 161, 149, 128, 77,  93,  61,  73,  17,
        128, 53,  5,   77,  81,  21,  73,  128, 185, 189, 221, 132, 0,
    };

    for (uint8_t i = 0; i < strlen(message); ++i) {
        message[i] = (message[i] >> 2) + (message[i] << 6);
    }

    char flag[] = "RXUS9Wx%R$fRaz2Df$r%`";
    for (uint8_t i = 0; i < strlen(flag); i++) {
        flag[i] = flag[i] ^ 0x14;
    }

    std::cout << message << ' ' << flag << std::endl;

    if (!Save::save_data.flag7) {
        Save::save_data.flag7 = true;
        Save::write_save();
    }

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
