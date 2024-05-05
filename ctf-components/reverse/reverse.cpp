#include "reverse.h"

static const char *TAG = "REVERSE";

void Reverse::encrypt() {
    for (int i = 0; i < 40; ++i) {
        // XOR the current character with the key
        Save::save_data.encrypted_cells[i] = Save::save_data.encrypted_cells[i] ^ 0x4f;

        // Subtract 10 from the result
        Save::save_data.encrypted_cells[i] = Save::save_data.encrypted_cells[i] - 10;
    }
    printf("Encrypted data: ");
    printf(Save::save_data.encrypted_cells);
    printf("\n");

}

void Reverse::decrypt() {
    printf("Not implemented\n");
}

int encrypt_cmd(int argc, char **argv) {

    Reverse* reverse = new Reverse;
    reverse->encrypt();
    return ESP_OK;
}

void register_encrypt_cmd(void) {
        const esp_console_cmd_t cmd = {
            .command = "encrypt",
            .help = "Encrypt cells\n",
            .hint = "",
            .func = &encrypt_cmd,
            .argtable = NULL,
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    }

int decrypt_cmd(int argc, char **argv) {

    Reverse* reverse = new Reverse;
    reverse->decrypt();
    return ESP_OK;
}

void register_decrypt_cmd(void) {
        const esp_console_cmd_t cmd = {
            .command = "decrypt",
            .help = "Decrypt cells\n",
            .hint = "",
            .func = &decrypt_cmd,
            .argtable = NULL,
        };
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    }