/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Patrick Downing <padraignix@gmail.com>
 */

#include "safe_unlock.h"

#define PIN_CLOCK     GPIO_NUM_2  // input (server -> client)
#define PIN_S2C_DATA  GPIO_NUM_7  // input (server -> client)
#define PIN_C2S_DATA  GPIO_NUM_6  // output (client -> server)

#define BIT_DELAY_US  30

#define QUANTUM_NAMESPACE "qkd"
static const char *TAG = "SAFE_UNLOCK";

typedef struct {
    uint8_t noisy_bits[129];
    uint8_t dock_basis[129];
    uint8_t dock_bits[129];
    uint8_t badge_basis[129];
    uint8_t ciphertext[129];
    uint8_t noisykey[129];
    uint8_t dockkey[129];
    uint8_t noisy_bits2[129];
    uint8_t dock_basis2[129];
    uint8_t dock_bits2[129];
    uint8_t badge_basis2[129];
    uint8_t ciphertext2[129];
    uint8_t noisykey2[129];
    uint8_t dockkey2[129];
} QKDDataSafe;

QKDDataSafe qkd_safe_data = {};

void get_qkdnvs_safe()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        return;
    }

    size_t data_size = sizeof(qkd_safe_data);
    err = nvs_get_blob(nvs_handle, "qkd_data", &qkd_safe_data, &data_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading Cascade data from NVS: %s\n", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
}

static void bus_init_safe(void)
{
    // CLOCK + S2C_DATA as inputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_CLOCK) | (1ULL << PIN_S2C_DATA),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    // C2S_DATA as output
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_C2S_DATA);
    gpio_config(&io_conf);
    // default low
    gpio_set_level(PIN_C2S_DATA, 0);
}

static void safe_read_bits(char *outBuf, size_t numBits)
{
    for (size_t i = 0; i < numBits; i++)
    {
        // Wait for clock to go HIGH
        while (gpio_get_level(PIN_CLOCK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        // Read the data line
        int bitVal = gpio_get_level(PIN_S2C_DATA);
        // Wait for clock to go LOW
        while (gpio_get_level(PIN_CLOCK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        outBuf[i] = bitVal ? '1' : '0';
    }
    outBuf[numBits] = '\0';
}

static void safe_send_bits(const char *bits)
{
    size_t len = strlen(bits);
    for (size_t i = 0; i < len; i++)
    {
        bool bitVal = (bits[i] == '1');
        gpio_set_level(PIN_C2S_DATA, bitVal ? 1 : 0);

        // The server toggles the clock, so we wait for one full cycle each bit
        // Wait for clock HIGH
        while (gpio_get_level(PIN_CLOCK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        // Wait for clock LOW
        while (gpio_get_level(PIN_CLOCK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    // Drive it low after done
    gpio_set_level(PIN_C2S_DATA, 0);
}

// XOR decrypt function
std::string decrypt_safe_flag() {

    get_qkdnvs_safe();

    if (strlen((char *)qkd_safe_data.ciphertext2) == 0) {
        printf("No ciphertext available - initialize Quantum link first.\n");
        return "nothing";
    }

    if (strlen((char *)qkd_safe_data.noisykey2) == 0) {
        printf("No key in storage - derive and enter QKD shared key.\n");
        return "nothing";
    }

    size_t cipher_len = strlen((char *)qkd_safe_data.ciphertext2);
    size_t key_len = strlen((char *)qkd_safe_data.noisykey2);
    uint8_t decrypted_binary[128 + 1] = {0};

    for (size_t i = 0; i < cipher_len; ++i) {
        decrypted_binary[i] = (qkd_safe_data.ciphertext2[i] ^ qkd_safe_data.noisykey2[i % key_len]) ? '1' : '0';
    }

    char decrypted_flag[128 / 8 + 1] = {0};
    size_t char_index = 0;
    for (size_t i = 0; i < cipher_len; i += 8) {
        char byte = 0;
        for (int bit = 0; bit < 8; ++bit) {
            byte = (byte << 1) | (decrypted_binary[i + bit] - '0');
        }
        decrypted_flag[char_index++] = byte;
    }

    std::string flag = decrypted_flag;
    printf("Sending Flag: %s\n\n", flag.c_str());
    return flag;
}

void string_to_binary(const char *input, char *output) {
    output[0] = '\0';

    for (size_t i = 0; i < strlen(input); ++i) {
        char binary[9];
        for (int bit = 7; bit >= 0; --bit) {
            binary[7 - bit] = ((input[i] >> bit) & 1) ? '1' : '0';
        }
        binary[8] = '\0';
        strcat(output, binary);
    }
}

static void safe_init(std::string flag)
{

    badge_print_text(0, (char*)"--Safe Unlock!--", 16, false);
    // To avoid complications keeping receiving initial step.
    // This helps with the badge being plugged in and controlling the flow
    badge_print_text(2, (char*)"SAO Lock Code", 13, false);

    char initdata[2];
    safe_read_bits(initdata, 1);

    // do nothing with it
    
    badge_print_text(2, (char*)"S: Send QKD2 Flg", 16, false);
    char flag_binary[129] = {0};
    string_to_binary(flag.c_str(), flag_binary);
    safe_send_bits(flag_binary);
    
    badge_print_text(2, (char*)"R: Unlock State6", 15, false);
    char unlockstate[2];
    safe_read_bits(unlockstate, 1);
    if (unlockstate[0] == '1') {
        badge_print_text(2, (char*)"Safe Unlocked!  ", 16, false);
    } else {
        badge_print_text(2, (char*)"Incorrect Flag! ", 16, false);
    }
    vTaskDelay(pdMS_TO_TICKS(1500));
}

int cmd_qkd_safe(int argc, char **argv)
{
    if (argc > 1) {
        printf("\nInvalid Safe Unlock Attempt\n");
    }
    else 
    {
        std::string flag = decrypt_safe_flag();
        if (flag == "nothing") {
            printf("No flag available - initialize Quantum link first.\n");
            return ESP_OK;
        }

        esp_log_level_set("gpio", ESP_LOG_WARN);
        printf("Attempting to unlock safe with QKD2 flag...\n");
        printf("Plug badge into safe and press A\n");
        printf("Alternatively, press the RESET button to reset and resume normal operations.\n");
        bus_init_safe();

        badge_ssd1306_clear();

        safe_init(flag);
    }
    return ESP_OK;
}

void register_qkd_safe_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "unlocksafe",
        .help = "Attempts to unlock safe with QKD2 flag",
        .hint = "",
        .func = &cmd_qkd_safe,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}