#include "qkd.h"

#define PIN_CLOCK     GPIO_NUM_2  // input (server -> client)
#define PIN_S2C_DATA  GPIO_NUM_7  // input (server -> client)
#define PIN_C2S_DATA  GPIO_NUM_6  // output (client -> server)

#define BIT_DELAY_US  50

#define QUANTUM_NAMESPACE "qkd"
static const char *TAG = "QKD";

// Define the structure for calibration data

typedef struct {
    uint8_t noisy_bits[129];
    uint8_t dock_basis[129];
    uint8_t dock_bits[129];
    uint8_t badge_basis[129];
    uint8_t ciphertext[129];
    uint8_t noisykey[129];
    uint8_t dockkey[129];
} QKDData;

QKDData qkd_data = {};

void print_qkdnvs_blob() {

    QKDData qkd_data2; // Temporary variable to hold the NVS data
    size_t required_size = sizeof(qkd_data2);

    // Try to get the blob from NVS
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK) {
        ESP_LOGE(TAG, "CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    }
    err2 = nvs_get_blob(nvs_handle, "qkd_data", &qkd_data2, &required_size);
    if (err2 == ESP_OK) {
        //printf("--- CHECKING - NVS QKD Data: ---\n");
        printf("  noisy_bits: %s\n", qkd_data2.noisy_bits);
        printf("  badge_basis: %s\n", qkd_data2.badge_basis);
        printf("  dock_basis: %s\n", qkd_data2.dock_basis);
        printf("  dock_bits: %s\n", qkd_data2.dock_bits);
        printf("  ciphertext: %s\n", qkd_data2.ciphertext);
        printf("  noisy_key: %s\n", qkd_data2.noisykey);
        printf("  dock_key: %s\n", qkd_data2.dockkey);          
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) { 
        printf("No QKD data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
}

void clear_qkdnvs_data() {
    nvs_handle_t handle;
    esp_err_t err;

    // Open NVS handle
    err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    // Erase the blob from NVS
    err = nvs_erase_key(handle, "qkd_data");
    if (err == ESP_OK) {
        printf("NVS data erased successfully.\n");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        //printf("No NVS data found to erase, initializing defaults.\n");
    } else {
        //printf("Error erasing NVS data: %s\n", esp_err_to_name(err));
    }

    // Write default calibration data back to NVS
    QKDData qkd_data = {};
    err = nvs_set_blob(handle, "qkd_data", &qkd_data, sizeof(qkd_data));
    if (err != ESP_OK) {
        //printf("Failed to write default calibration data to NVS: %s\n", esp_err_to_name(err));
    } else {
        //printf("Default calibration data written to NVS.\n");
    }

    // Commit changes
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s\n", esp_err_to_name(err));
    } else {
       // printf("NVS changes committed.\n");
    }

    // Close NVS handle
    nvs_close(handle);
}

void update_qkdnvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Store the calibration data structure as a single blob
    err = nvs_set_blob(nvs_handle, "qkd_data", &qkd_data, sizeof(qkd_data));
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // If no data found, initialize with defaults
        //printf("No calibration data found in NVS. Initializing with default values...\n");
    } else if (err == ESP_OK) {
        //printf("Calibration data loaded from NVS successfully.\n");
    } else {
        ESP_LOGE(TAG, "Error reading NVS: %s\n", esp_err_to_name(err));
    }

    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit changes to NVS: %s\n", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}

void get_qkdnvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }

    // Retrieve the calibration data structure
    size_t data_size = sizeof(qkd_data);
    err = nvs_get_blob(nvs_handle, "qkd_data", &qkd_data, &data_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        //printf("Calibration data not found. Using default values.\n");
        QKDData qkd_data = {};
        update_qkdnvs();
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading calibration data from NVS: %s\n", esp_err_to_name(err));
    } else {
        //printf("Calibration data retrieved successfully.\n");
    }

    nvs_close(nvs_handle);
}

static void bus_init(void)
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

static void client_read_bits(char *outBuf, size_t numBits)
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

static void client_send_bits(const char *bits)
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

void generate_random_bit_string(char *buffer, int length) {
    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < length; ++i) {
        buffer[i] = (rand() % 2) ? '1' : '0';  // Add '0' or '1' to the buffer
    }
    buffer[length] = '\0';  // Null-terminate the string
}

// Function to XOR a UTF-8 string with a shared key
void xor_with_key(const char *input, const char *key, char *output) {
    size_t input_len = strlen(input);
    size_t key_len = strlen(key);

    if (key_len == 0) {
        printf("Error: Key length is zero!\n");
        output[0] = '\0';  // Return an empty string in case of error
        return;
    }

    // XOR each character of the input with the key
    for (size_t i = 0; i < input_len; ++i) {
        output[i] = input[i] ^ key[i % key_len];  // Repeat the key if necessary
    }

    // Null-terminate the output string
    output[input_len] = '\0';
}

// Function to generate the shared key from basis bits
void generate_key_from_basis(const char *key, const char *basis1, const char *basis2, char *shared_key) {
    int j = 0;  // Index for the shared key
    // Ensure all strings are of the same length
    if (strlen(key) != strlen(basis1) || strlen(basis1) != strlen(basis2)) {
        printf("Error: Input strings must have the same length!\n");
        shared_key[0] = '\0';  // Return an empty string in case of error
        return;
    }
    // Loop through each bit and compare the basis
    for (size_t i = 0; i < strlen(key); ++i) {
        if (basis1[i] == basis2[i]) {
            // Basis match, keep the corresponding key bit
            shared_key[j++] = key[i];
        }
    }
    // Null-terminate the shared key string
    shared_key[j] = '\0';
}

static void qkd_init(void)
{
    // Receive noisybits
    char noisybits[129];
    client_read_bits(noisybits, 128);
    std::memcpy(qkd_data.noisy_bits, noisybits, sizeof(qkd_data.noisy_bits));
    printf("noisy_key: %s\n", noisybits);

    // Send badge_basis
    // TODO: Random string of 128 1/0s
    char pongMsg[129];
    generate_random_bit_string(pongMsg, 128);
    printf("badge_basis: %s\n", pongMsg);
    std::memcpy(qkd_data.badge_basis, pongMsg, sizeof(qkd_data.badge_basis));
    client_send_bits(pongMsg);

    // Receive dock_basis
    char dockbasis[129];
    client_read_bits(dockbasis, 128);
    std::memcpy(qkd_data.dock_basis, dockbasis, sizeof(qkd_data.dock_basis));
    printf("dock_basis: %s\n", dockbasis);

    // Receive dockbits
    char dockbit[129];
    client_read_bits(dockbit, 128);
    std::memcpy(qkd_data.dock_bits, dockbit, sizeof(qkd_data.dock_bits));
    printf("dock_key: %s\n", dockbit);

    // TODO: get perfect and noisy keys based on above data
    // Insert noisy key
    // Insert dock key

    // Receive dock_ciphertext
    char dockcipher[129];
    client_read_bits(dockcipher, 128);
    std::memcpy(qkd_data.ciphertext, dockcipher, sizeof(qkd_data.ciphertext));
    printf("ciphertext: %s\n", dockcipher);

    char shared_key[129];
    generate_key_from_basis(dockbit, pongMsg, dockbasis, shared_key);
    std::memcpy(qkd_data.dockkey, shared_key, sizeof(qkd_data.dockkey));
    printf("shared_key: %s\n", shared_key);

    update_qkdnvs();
    get_qkdnvs();

    badge_ssd1306_clear();
    badge_print_text(1, "Transfer Done!", 16, false);
    badge_print_text(1, "Quantumly Linked", 16, false);
}

/*
// Interactive parity check function
void parity_check_round(QKDData& qkd_data, const uint8_t* perfect_key, int block_size) {
    int blocks = 16 / block_size;
    for (int i = 0; i < blocks; i++) {
        int start = i * block_size;
        int end = start + block_size;

        // Ask the player for the parity of this block
        std::cout << "\nBlock " << i + 1 << " (" << start << "-" << end - 1 << ") - ";
        std::cout << "Input the calculated parity (0 or 1): ";
        int player_parity;
        std::cin >> player_parity;

        // Calculate the actual parity for comparison
        int actual_parity = calculate_parity(perfect_key, start, end);

        if (player_parity != actual_parity) {
            std::cout << "Mismatch detected! Starting binary search to locate the error...\n";
            int error_index = start;
            int low = start, high = end - 1;

            // Binary search to find the exact bit error
            while (low < high) {
                int mid = (low + high) / 2;
                int left_parity = calculate_parity(perfect_key, low, mid + 1);
                std::cout << "Checking parity for range [" << low << "-" << mid << "]... Enter parity: ";
                std::cin >> player_parity;

                if (player_parity != left_parity) {
                    high = mid;
                } else {
                    low = mid + 1;
                }
            }

            error_index = low;
            std::cout << "Error found at bit " << error_index << ". Correcting...\n";

            // Correct the bit in the noisy key
            qkd_data.noisy_key[error_index] ^= 1;
        }
    }
}

// Main interactive function
void interactive_cascade_protocol(QKDData& qkd_data) {
    uint8_t perfect_key[64];
    generate_noisy_key(qkd_data.noisy_key, perfect_key);

    std::cout << "Initial keys generated. Here are the keys:\n";
    print_key(qkd_data.noisy_key, "Noisy Key");
    print_key(perfect_key, "Perfect Key");

    // Perform parity checks in multiple rounds with decreasing block sizes
    std::vector<int> block_sizes = {16, 8, 4};  // Example block sizes for rounds
    for (int block_size : block_sizes) {
        std::cout << "\nStarting parity check round with block size " << block_size << "...\n";
        parity_check_round(qkd_data, perfect_key, block_size);
    }

    // Final corrected key result
    std::cout << "\nFinal corrected key:\n";
    print_key(qkd_data.noisy_key, "Corrected Key");

    // Compare to perfect key to determine success
    if (std::memcmp(qkd_data.noisy_key, perfect_key, 64) == 0) {
        std::cout << "\nCongratulations! You have successfully corrected the key.\n";
    } else {
        std::cout << "\nThe key is still incorrect. Try again.\n";
    }
}
*/

// Prompt player for the derived key and save it in NVS
void input_and_store_key() {
    char *input_key = linenoise("Enter the derived key: ");
    if (input_key == nullptr) {
        printf("Error reading input.\n");
        return;
    }
    if (strlen(input_key) > 129) {
        printf("Key is too long. Max length is %d bits.\n", 128);
        free(input_key);
        return;
    }
    strncpy((char *)qkd_data.noisykey, input_key, 128);
    update_qkdnvs();
    get_qkdnvs();
    printf("Key saved successfully!\n");
    free(input_key);
}
// XOR decrypt function
void decrypt_flag() {

   // printf("Test?\n");
    get_qkdnvs();
    update_qkdnvs();
    //printf((char *)qkd_data.ciphertext);
    //printf("Test?\n");
    //for (int i = 0; i < 128; ++i) {
    //    printf("%02X ", qkd_data.ciphertext[i]);
    //}
    //printf("Test?\n");
    if (strlen((char *)qkd_data.ciphertext) == 0) {
        printf("No ciphertext available - initialize Quantum link first.\n");
        return;
    }

    if (strlen((char *)qkd_data.noisykey) == 0) {
        printf("No key in storage - derive and enter QKD shared key.\n");
        input_and_store_key();
    }

    // Perform XOR decryption with wrapping key
    size_t cipher_len = strlen((char *)qkd_data.ciphertext);
    size_t key_len = strlen((char *)qkd_data.noisykey);
    uint8_t decrypted_binary[128 + 1] = {0};

    for (size_t i = 0; i < cipher_len; ++i) {
        decrypted_binary[i] = (qkd_data.ciphertext[i] ^ qkd_data.noisykey[i % key_len]) ? '1' : '0';
    }

    // Convert binary back to string
    char decrypted_flag[128 / 8 + 1] = {0};
    size_t char_index = 0;
    for (size_t i = 0; i < cipher_len; i += 8) {
        char byte = 0;
        for (int bit = 0; bit < 8; ++bit) {
            byte = (byte << 1) | (decrypted_binary[i + bit] - '0');
        }
        decrypted_flag[char_index++] = byte;
    }

    printf("Decrypted flag: %s\n", decrypted_flag);
}


int cmd_qkd(int argc, char **argv)
{

    if (strcmp(argv[1], "init") == 0) {
        //printf("\n--- Init QKD WorkFlow ---\n");
        printf("Starting Quantum Linking with Dock and QKD data exchange...\n");
        bus_init();

        badge_ssd1306_clear();
        badge_print_text(0, "Init QKD Flow", 16, false);
        badge_print_text(1, "Plug in dock", 16, false);
        badge_print_text(2, "and press", 16, false);
        badge_print_text(3, "A to start", 16, false);

        qkd_init();
    }
    else if (strcmp(argv[1], "list") == 0) {
        print_qkdnvs_blob();
    }
    else if (strcmp(argv[1], "decrypt") == 0) {
        decrypt_flag();
    }
    else if (strcmp(argv[1], "clear") == 0) {
        clear_qkdnvs_data();
    }
    else if (strcmp(argv[1], "cascade") == 0) {
        printf("\n--- Cascade QKD WorkFlow ---\n");
    }
    else {
        printf("\nInvalid QKD command\n");
    }

    return ESP_OK;
}

void register_qkd_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "qkd",
        .help = "Initiate QKD Key Exchange with dock\n",
        .hint = "init | list | clear | cascade | decrypt",
        .func = &cmd_qkd,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}