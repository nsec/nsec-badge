/*
 * 2024 Hugo Genesse
 *
 */

#pragma once

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"

#include "save.h"

#ifdef __cplusplus
extern "C" {
#endif

class Reverse {

public:
    Reverse() {
        memset(Save::save_data.encrypted_cells, 0, 50);
        strcpy(Save::save_data.encrypted_cells,
        "\xff\xf9\x04\xfe\x58\x21\x6d\x6f\x6f\x71\x22\x75\x72\x6d\x21\x23\x1f\x21\x6c\x73\x6c\x6f\x1f\x71\x24\x72\x6c\x6e\x6d\x74\x71\x24\x70\x6e\x6d\x24\x70");
    }

    virtual ~Reverse() {}
    void encrypt();
    void decrypt();
};

void register_encrypt_cmd(void);
void register_decrypt_cmd(void);

#ifdef __cplusplus
}
#endif
