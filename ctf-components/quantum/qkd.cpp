/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2025 Patrick Downing <padraignix@gmail.com>
 */

 #include "qkd.h"

#define PIN_CLOCK     GPIO_NUM_2  // input (server -> client)
#define PIN_S2C_DATA  GPIO_NUM_7  // input (server -> client)
#define PIN_C2S_DATA  GPIO_NUM_6  // output (client -> server)

#define BIT_DELAY_US  30

#define QUANTUM_NAMESPACE "qkd"
static const char *TAG = "QKD";

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
} QKDData;

QKDData qkd_data = {};

QKDData default_qkd_data = {
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}
};

void update_qkdnvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    }
    err = nvs_set_blob(nvs_handle, "qkd_data", &qkd_data, sizeof(qkd_data));
    if (err == ESP_OK) {
        //printf("Calibration data loaded from NVS successfully.\n");
    } else {
        ESP_LOGE(TAG, "Error reading NVS: %s\n", esp_err_to_name(err));
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit changes to NVS: %s\n", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
}

void print_qkdnvs_blob() {
    QKDData qkd_data2;
    size_t required_size = sizeof(qkd_data2);
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK && err2 != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(nvs_handle);
        update_qkdnvs();
        printf("Quantum Data was empty - initialized, you can now re-run list.\n");
        return;
    }

    err2 = nvs_get_blob(nvs_handle, "qkd_data", &qkd_data2, &required_size);
    if (err2 == ESP_OK) {
            printf("  quantum_bits: %s\n", qkd_data2.dock_bits);
            printf("  badge_basis: %s\n", qkd_data2.badge_basis);
            printf("  dock_basis: %s\n", qkd_data2.dock_basis);
            printf("  ciphertext: %s\n", qkd_data2.ciphertext);  
            printf("  derived_key: %s\n", qkd_data2.noisykey); 
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) { 
        printf("No QKD data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
    nvs_close(nvs_handle);
}

void print_qkdnvs_blobnoisy() {

    QKDData qkd_data2;
    size_t required_size = sizeof(qkd_data2);
    nvs_handle_t nvs_handle;
    esp_err_t err2 = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err2 != ESP_OK && err2 != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "CHECKING - Failed to open NVS namespace: %s\n", esp_err_to_name(err2));
        return;
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(nvs_handle);
        update_qkdnvs();
        printf("Quantum Data was empty - initialized, you can now re-run list.\n");
        return;
    }

    err2 = nvs_get_blob(nvs_handle, "qkd_data", &qkd_data2, &required_size);
    if (err2 == ESP_OK) {
            //printf("  noisy_bits: %s\n", qkd_data2.noisy_bits2);
            //printf("  badge_basis: %s\n", qkd_data2.badge_basis2);
            //printf("  dock_basis: %s\n", qkd_data2.dock_basis2);
            printf("  ciphertext: %s\n", qkd_data2.ciphertext2);   
            printf("  derived_key: %s\n", qkd_data2.noisykey2);
            //printf("  [ADMIN] perfect_key: %s\n", qkd_data2.dockkey2);
    } else if (err2 == ESP_ERR_NVS_NOT_FOUND) { 
        printf("No QKD data found in NVS.\n");
    } else {
        printf("Error reading NVS blob: %s\n", esp_err_to_name(err2));
    }
    nvs_close(nvs_handle);
}

void clear_qkdnvs_data() {
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(QUANTUM_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    err = nvs_erase_key(handle, "qkd_data");
    if (err == ESP_OK) {
        printf("Quantum data erased successfully.\n");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        //printf("No NVS data found to erase, initializing defaults.\n");
    } else {
        //printf("Error erasing NVS data: %s\n", esp_err_to_name(err));
    }
    QKDData qkd_data = {};
    err = nvs_set_blob(handle, "qkd_data", &qkd_data, sizeof(qkd_data));
    if (err != ESP_OK) {
        //printf("Failed to write default calibration data to NVS: %s\n", esp_err_to_name(err));
    } else {
        //printf("Default calibration data written to NVS.\n");
    }
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s\n", esp_err_to_name(err));
    } else {
       // printf("NVS changes committed.\n");
    }
    nvs_close(handle);
}

void get_qkdnvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(QUANTUM_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s\n", esp_err_to_name(err));
        return;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        update_qkdnvs();
        return;
    }

    size_t data_size = sizeof(qkd_data);
    err = nvs_get_blob(nvs_handle, "qkd_data", &qkd_data, &data_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading Cascade data from NVS: %s\n", esp_err_to_name(err));
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
    srand(time(NULL));
    for (int i = 0; i < length; ++i) {
        buffer[i] = (rand() % 2) ? '1' : '0';
    }
    buffer[length] = '\0';
}

void xor_with_key(const char *input, const char *key, char *output) {
    size_t input_len = strlen(input);
    size_t key_len = strlen(key);

    if (key_len == 0) {
        printf("Error: Key length is zero!\n");
        output[0] = '\0';
        return;
    }
    for (size_t i = 0; i < input_len; ++i) {
        output[i] = input[i] ^ key[i % key_len];
    }
    output[input_len] = '\0';
}

void generate_key_from_basis(const char *key, const char *basis1, const char *basis2, char *shared_key) {
    int j = 0;
    if (strlen(key) != strlen(basis1) || strlen(basis1) != strlen(basis2)) {
        printf("Error: Input strings must have the same length!\n");
        shared_key[0] = '\0';
        return;
    }
    for (size_t i = 0; i < strlen(key); ++i) {
        if (basis1[i] == basis2[i]) {
            shared_key[j++] = key[i];
        }
    }
    shared_key[j] = '\0';
}

static void update_display_progress(const char* status, int step, int total_steps)
{
    char progress[17] = {0};
    char percentage[17] = {0};
    int percent = (step * 100) / total_steps;
    
    int bar_length = 16;
    int filled = (step * bar_length) / total_steps;
    
    for (int i = 0; i < bar_length; i++) {
        progress[i] = (i < filled) ? '*' : '-';
    }
    progress[bar_length] = '\0';
    
    snprintf(percentage, sizeof(percentage), "Progress: %d%%", percent);
    
    badge_ssd1306_clear();
    badge_print_text(0, (char*)"Quantum Link", 12, false);
    badge_print_text(1, (char*)status, strlen(status), false);
    badge_print_text(2, (char*)progress, strlen(progress), false);
    badge_print_text(3, (char*)percentage, strlen(percentage), false);
}

static void qkd_init(void)
{
    const int total_steps = 10;
    int current_step = 0;
    
    // Receive noisybits
    update_display_progress("Press A to Start", current_step, total_steps);
    
    char noisybits[129];
    client_read_bits(noisybits, 128);
    std::memcpy(qkd_data.noisy_bits, noisybits, sizeof(qkd_data.noisy_bits));
    update_display_progress("R: Dock Qubits", ++current_step, total_steps);

    // Send badge_basis
    
    char pongMsg[129];
    generate_random_bit_string(pongMsg, 128);
    std::memcpy(qkd_data.badge_basis, pongMsg, sizeof(qkd_data.badge_basis));
    client_send_bits(pongMsg);
    update_display_progress("S: Badge basis", ++current_step, total_steps);

    // Receive dock_basis
    
    char dockbasis[129];
    client_read_bits(dockbasis, 128);
    std::memcpy(qkd_data.dock_basis, dockbasis, sizeof(qkd_data.dock_basis));
    update_display_progress("R: Dock basis", ++current_step, total_steps);

    // Receive dockbits
    
    char dockbit[129];
    client_read_bits(dockbit, 128);
    std::memcpy(qkd_data.dock_bits, dockbit, sizeof(qkd_data.dock_bits));
    char dockcipher[129];
    client_read_bits(dockcipher, 128);
    std::memcpy(qkd_data.ciphertext, dockcipher, sizeof(qkd_data.ciphertext));
    update_display_progress("R: Ciphertext", ++current_step, total_steps);

    char shared_key[129];
    generate_key_from_basis(dockbit, pongMsg, dockbasis, shared_key);
    std::memcpy(qkd_data.dockkey, shared_key, sizeof(qkd_data.dockkey));

    /* Redo transfer, with noisy set now */

    // Receive noisybits - what the player will need to correct
    
    char noisybits2[129];
    client_read_bits(noisybits2, 128);
    std::memcpy(qkd_data.noisy_bits2, noisybits2, sizeof(qkd_data.noisy_bits2));
    update_display_progress("R: Noisy Qubits", ++current_step, total_steps);
    
    char pongMsg2[129];
    generate_random_bit_string(pongMsg2, 128);
    std::memcpy(qkd_data.badge_basis2, pongMsg2, sizeof(qkd_data.badge_basis2));
    client_send_bits(pongMsg2);
    update_display_progress("S: Badge basis 2", ++current_step, total_steps);

    // Receive dock_basis - randomly chosen basis
    char dockbasis2[129];   
    client_read_bits(dockbasis2, 128);
    std::memcpy(qkd_data.dock_basis2, dockbasis2, sizeof(qkd_data.dock_basis2));
    update_display_progress("R: Dock Basis 2", ++current_step, total_steps);

    // Receive dockbits - non-noisy set for calibrate comparison
    char dockbit2[129];
    client_read_bits(dockbit2, 128);
    std::memcpy(qkd_data.dock_bits2, dockbit2, sizeof(qkd_data.dock_bits2));

    // Receive dock_ciphertext - encrypted using non-noisy set
    char dockcipher2[129];
    client_read_bits(dockcipher2, 128);
    std::memcpy(qkd_data.ciphertext2, dockcipher2, sizeof(qkd_data.ciphertext2));
    update_display_progress("R: Ciphertext 2", ++current_step, total_steps);

    // Generate shared key from non-noisy set, for cascade comparison
    char shared_key2[129];
    generate_key_from_basis(dockbit2, pongMsg2, dockbasis2, shared_key2);
    std::memcpy(qkd_data.dockkey2, shared_key2, sizeof(qkd_data.dockkey2));
    
    // Process finished - now add noise
    
    update_display_progress("Processing data", ++current_step, total_steps);

    char noisy_shared_key2[129];
    std::memcpy(noisy_shared_key2, shared_key2, sizeof(noisy_shared_key2));
    
    //Commenting out the "true random" error introduction for CTF purpose of specific error
    /*srand((unsigned)time(NULL));
    for (int i = 0; i < strlen((char *)noisy_shared_key2) - 1 ; i++) {
        double r = (double)rand() / (double)RAND_MAX; // random in [0,1)
        if(r < 0.10) {  // 10% threshold
            // Flip the bit
            if(noisy_shared_key2[i] == '0') {
                noisy_shared_key2[i] = '1';
            } else if(noisy_shared_key2[i] == '1') {
                noisy_shared_key2[i] = '0';
            }
        }
    }
    */
    
    //Need to flip specific bits in the shared key, to enforce the player to use Cascade
    //and specifically have to run through at least a single iteration of shuffle
    //This assumes a key around ~64 bits, and QBER of 10%, so need to flip 6-7 bits
    
    //int keysize = strlen((char *)noisy_shared_key2) - 1;

    // Iterate through specific indexes to be modified/flipped
    // Indexes: block 1: 5,6
    //          block 2: 14 
    //          block 3: 18,20
    //          block 4: 25 
    //          block 6: 46

    for (int i : {5,6,14,18,20,25,46})
    {
        if(noisy_shared_key2[i] == '0') {
            noisy_shared_key2[i] = '1';
        } else if(noisy_shared_key2[i] == '1') {
            noisy_shared_key2[i] = '0';
        }
    }

    std::memcpy(qkd_data.noisykey2, noisy_shared_key2, sizeof(qkd_data.noisykey2));

    /*------- end of Noisy set -------*/

    update_display_progress("Saving data", current_step, total_steps);

    std::memset(qkd_data.noisykey, 0, sizeof(qkd_data.noisykey));

    update_qkdnvs();

    update_display_progress("Process complete", total_steps, total_steps);
    vTaskDelay(pdMS_TO_TICKS(1500));
    
    badge_ssd1306_clear();
    badge_print_text(0, (char*)"Quantum Link", 12, false);
    badge_print_text(1, (char*)"Complete!", 10, false);
    badge_print_text(2, (char*)"Run 'qkd decrypt'", 17, false);
    badge_print_text(3, (char*)"to continue", 11, false);
}


int computeParity(const std::vector<int>& bits, int start, int end) {
    int sum = 0;
    for(int i = start; i <= end; ++i) {
        sum += bits[i];
    }
    return sum % 2;
}

/**
 * Recursively locate one erroneous bit within localKey[start..end].
 * - keys might be shuffled relative to original positions.
 * - indicesMap[i] = the original index of the bit currently at position i.
 * Once we find exactly one bit, we show the *original* index to the player
 * so they can flip it in the unshuffled coordinate system.
 */
void binarySearchError(const std::vector<int>& badgeKey, const std::vector<int>& dockKey, const std::vector<int>& indicesMap, int start, int end) 
{
    if(start == end) {
        if (badgeKey[start] != dockKey[start]) {
        int originalIdx = indicesMap[start];
        printf("==> Potential Error found bit at index %i!\n\n",originalIdx);
        return;
        } else {
            printf("No error found in this sub-block.\n");
            return;
        }
    }

    int mid = (start + end) / 2;

    printf("Block Index [%i,%i]\n",start,mid);
    printf("Left block bits: ");
    for (int i = start; i <= mid; i++) {
        printf("%d", badgeKey[i]);
    }
    printf("\n");

    // Prompt user for "badge parity" of the LEFT half [start..mid]
    int leftParityBadge = 0;
    {
        char* line = linenoise("Enter parity for left half: ");
        if(!line) {
            printf("User cancelled input. Exiting.\n");
            return;
        }
        leftParityBadge = std::atoi(line);
        linenoiseFree(line);
    }

    // Compare with "dock parity" for the same sub-block
    int leftParityDock = computeParity(dockKey, start, mid);
    if(leftParityDock != leftParityBadge) {
        // Mismatch => error is in the left half
        printf("=> Mismatch in Left sub-block searching.\n");
        binarySearchError(badgeKey, dockKey, indicesMap, start, mid);
    } else {
        // Must be in the right half
        printf("=> Mismatch in Right sub-block searching.\n");
        binarySearchError(badgeKey, dockKey, indicesMap, mid+1, end);
    }
}

/**
 * Run a single pass of Cascade:
 * 1) Ask user if they want to shuffle the key
 * 2) Possibly shuffle keys + indicesMap
 * 3) Partition the (shuffled) bits into blocks
 * 4) For each block, ask user for server parity
 * 5) If mismatch, do a binarySearchError to find the erroneous bit
 */
void runCascadeFlow() {
    
    get_qkdnvs();
    update_qkdnvs();

    const char* badgeString = reinterpret_cast<const char*>(qkd_data.noisykey2);

    if(!badgeString) {
        printf("Cascade Setup Error\n");
        return;
    }

    int size = strlen(badgeString) - 1;
    if(size < 1) {
        printf("Quantum Data seem empty\n");
        return;
    }
    std::vector<int> badgeKey;
    badgeKey.reserve(size);

    for(int i = 0; i < size; i++) {
        if(badgeString[i] == '0') {
            badgeKey.push_back(0);
        } else if(badgeString[i] == '1') {
            badgeKey.push_back(1);
        } else {
            printf("Invalid character '%c' in bitstring (index %d)\n", badgeString[i], i);
            return;
        }
    }

    std::vector<int> indicesMap(badgeKey.size());
    for(size_t i=0; i<badgeKey.size(); i++) {
        indicesMap[i] = i;
    }

    const char* dockString = reinterpret_cast<const char*>(qkd_data.dockkey2);
    if(!dockString) {
        printf("Cascade Setup Error\n");
        return;
    }
    std::vector<int> dockKey;
    dockKey.reserve(size);

    for(int i = 0; i < size; i++) {
        if(dockString[i] == '0') {
            dockKey.push_back(0);
        } else if(dockString[i] == '1') {
            dockKey.push_back(1);
        } else {
            printf("Invalid character '%c' in bitstring (index %d)\n", dockString[i], i);
            return;
        }
    }

    char* line = linenoise("Do you want to shuffle the derived key? (y/n): ");
    if(!line) {
        printf("User cancelled.\n");
        return;
    }
    std::string answer(line);
    linenoiseFree(line);

    if(answer == "yes" || answer == "y") {
        std::random_shuffle(indicesMap.begin(), indicesMap.end());
        std::vector<int> shuffledBadgeKey(size);
        std::vector<int> shuffledDockKey(size);

        for(int i = 0; i < size; i++) {
            shuffledBadgeKey[i] = badgeKey[indicesMap[i]];
            shuffledDockKey[i] = dockKey[indicesMap[i]];
        }
        badgeKey = shuffledBadgeKey;
        dockKey = shuffledDockKey;
        printf("Key and index map have been shuffled.\n");
    }

    // For this example, let's define a block size in some simple manner.
    // In real usage might base it on QBER or logic from earlier steps.
    int blockSize = 8;
    int numBlocks = (size + blockSize - 1) / blockSize;

    printf("\n-- Starting Cascade pass with blockSize=%i, numBlocks=%i\n", blockSize, numBlocks);

    bool foundAnyError = false;

    for(int blockIndex = 0; blockIndex < numBlocks; blockIndex++) {
        int start = blockIndex * blockSize;
        int end = std::min(start + blockSize - 1, size - 1);

        // Ask user for "badge parity" for this sub-block
        printf("Block %i: local indices [%i,%i]\n",blockIndex,start,end);

        printf("Block bits: ");
        for (int i = start; i <= end; i++) {
            printf("%d", badgeKey[i]);
        }
        printf("\n");

        char* line = linenoise("Enter parity for this block (0 or 1): ");
        if(!line) {
            printf("User cancelled input.\n");
            return;
        }
        int badgeParity = std::atoi(line);
        linenoiseFree(line);
        if(badgeParity != 0 && badgeParity != 1) {
            printf("Invalid parity input. Skipping block.\n");
            return;
        }

        // Compute local parity to see if there's a mismatch
        int dockParity = computeParity(dockKey, start, end);

        if(badgeParity != dockParity) {
            printf("Parity mismatch => At least one error in this block.\n");
            foundAnyError = true;
            // Recursively locate that error
            binarySearchError(badgeKey, dockKey, indicesMap, start, end);
        } else {
            printf("Parity matches => No error in this block.\n");
        }
    }

    if(!foundAnyError) {
        printf("\nNo bit errors found in this pass.\n");
    } else {
        printf("\nSome errors were found. Bits need flipping (via separate function).\n");
    }
}

void input_and_store_key2() {
    printf("You now need to derive the key from the QKD exchange.\n");
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
    strncpy((char *)qkd_data.noisykey2, input_key, 128);
    update_qkdnvs();
    get_qkdnvs();
    printf("Key saved successfully!\n");
    free(input_key);
}

void input_and_store_key() {
    printf("You now need to derive the key from the QKD exchange.\n");
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

void decrypt_flag2() {

    get_qkdnvs();
    update_qkdnvs();

    if (strlen((char *)qkd_data.ciphertext2) == 0) {
        printf("No ciphertext available - initialize Quantum link first.\n");
        return;
    }

    if (strlen((char *)qkd_data.noisykey2) == 0) {
        printf("No key in storage - derive and enter QKD shared key.\n");
        input_and_store_key2();
    } else {
        printf("Shared key already derived and stored in NVS.\n");
        // create function to ask if they want to overwrite the key
        char *input_option = linenoise("Key Detected, do you want to override key? (y/n): ");
        if (input_option == nullptr || strlen(input_option) > 2) {
            printf("Error reading input.\n");
            return;
        }
        if (strcmp(input_option, "y") == 0) {
            input_and_store_key2();
            free(input_option);
            //return;
        } else {
            printf("Key not overwritten.\n");
            free(input_option);
        }
    }

    size_t cipher_len = strlen((char *)qkd_data.ciphertext2);
    size_t key_len = strlen((char *)qkd_data.noisykey2);
    uint8_t decrypted_binary[128 + 1] = {0};

    for (size_t i = 0; i < cipher_len; ++i) {
        decrypted_binary[i] = (qkd_data.ciphertext2[i] ^ qkd_data.noisykey2[i % key_len]) ? '1' : '0';
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

    // Required to pass CI checker
    std::string obfFlag = "\x55\x5F\x52\x54\x3E";  
    std::string flagstr;
    flagstr.reserve(obfFlag.size());
    for (char c : obfFlag) {
        flagstr.push_back(c ^ 0x13);
    }

    std::string flag = flagstr + decrypted_flag;

    printf("Decrypted flag: %s\n", flag.c_str());
}

void decrypt_flag() {

    get_qkdnvs();
    update_qkdnvs();

    if (strlen((char *)qkd_data.ciphertext) == 0) {
        printf("No ciphertext available - initialize Quantum link first.\n");
        return;
    }

    if (strlen((char *)qkd_data.noisykey) == 0) {
        printf("No key in storage - derive and enter QKD shared key.\n");
        input_and_store_key();
    } else {
        printf("Shared key already derived and stored in NVS.\n");
        char *input_option = linenoise("Key Detected, do you want to override key? (y/n): ");
        if (input_option == nullptr || strlen(input_option) > 2) {
            printf("Error reading input.\n");
            return;
        }
        if (strcmp(input_option, "y") == 0) {
            input_and_store_key();
            free(input_option);
        } else {
            printf("Key not overwritten.\n");
            free(input_option);
        }
    }

    size_t cipher_len = strlen((char *)qkd_data.ciphertext);
    size_t key_len = strlen((char *)qkd_data.noisykey);
    uint8_t decrypted_binary[128 + 1] = {0};

    for (size_t i = 0; i < cipher_len; ++i) {
        decrypted_binary[i] = (qkd_data.ciphertext[i] ^ qkd_data.noisykey[i % key_len]) ? '1' : '0';
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

    // Required to pass CI checker
    std::string obfFlag = "\x55\x5F\x52\x54\x3E";  
    std::string flagstr;
    flagstr.reserve(obfFlag.size());
    for (char c : obfFlag) {
        flagstr.push_back(c ^ 0x13);
    }

    std::string flag = flagstr + decrypted_flag;

    printf("Decrypted flag: %s\n", flag.c_str());
}

void qkd_mock_data(){
    const char *quantum_bits =  "01111001011100101010001101101100011011101010001100010011001100001100001111011001000100101110111100101100011001100101100000110101";
    const char *badge_basis  =  "01011110011110010001010011011110100011111100011000110001111110010011001111110111100000010100011011101111001110001001101010101010";
    const char *dock_basis   =  "11110100010001000111100000000000011000110010111010111001000111101000001111100011110111100010101001011110011011101010010101100011";
    const char *ciphertext   =  "10000110001110010111011010100001100011110011001001010000101011101011110100100011001111100100100000011001010101110011111101111100";

    const char *input_bits2  =  "111011101001110100100000001100011101011001100011111100000101001000";
    const char *dock_key2   =   "111010001001111100001000011100011101011001100001111100000101001000";
    const char *ciphertext2  =  "10100110111100000011100100000010101011110011000010111011000101100111111101011111101000010111010000110001101010000001001001110001";

    std::memcpy(qkd_data.dock_basis, dock_basis, sizeof(qkd_data.dock_basis));
    std::memcpy(qkd_data.dock_bits, quantum_bits, sizeof(qkd_data.dock_bits));
    std::memcpy(qkd_data.badge_basis, badge_basis, sizeof(qkd_data.badge_basis));
    std::memcpy(qkd_data.ciphertext, ciphertext, sizeof(qkd_data.ciphertext));

    std::memcpy(qkd_data.ciphertext2, ciphertext2, sizeof(qkd_data.ciphertext2));
    std::memcpy(qkd_data.noisykey2, input_bits2, sizeof(qkd_data.noisykey2));
    std::memcpy(qkd_data.dockkey2, dock_key2, sizeof(qkd_data.dockkey2));

    update_qkdnvs();
    get_qkdnvs();
    printf("QKD mock data loaded.\n");
}

int cmd_qkd(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            print_qkdnvs_blob();
        }
        else if (strcmp(argv[1], "decrypt") == 0) {
            decrypt_flag();
        }
        else {
            printf("\nInvalid QKD command\n");
        }
    }
    else
    {
        printf("\nInvalid QKD command\n");
    }
    return ESP_OK;
}

int cmd_qkd2(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            print_qkdnvs_blobnoisy();
        }
        else if (strcmp(argv[1], "decrypt") == 0) {
            decrypt_flag2();
        }
        else if (strcmp(argv[1], "cascade") == 0) {
            runCascadeFlow();
        }
        else {
            printf("\nInvalid QKD command\n");
        }
    } 
    else
    {
        printf("\nInvalid QKD command\n");
    }
    return ESP_OK;
}

int cmd_qkd_init(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "clear") == 0) 
        {
            clear_qkdnvs_data();
        }
        else if (strcmp(argv[1], "mock") == 0) 
        {
            printf("Setting up post NSEC mock QKD data...\n");
            qkd_mock_data();
        }
        else 
        {
            printf("\nInvalid QKD-Init command\n");
        }
    }
    else 
    {
        esp_log_level_set("gpio", ESP_LOG_WARN);
        printf("Starting Quantum Linking with Dock and QKD data exchange...\n");
        printf("Let the exchange complete before resuming.\n");
        printf("Alternatively, press the RESET button to reset and resume normal operations.\n");
        bus_init();

        badge_ssd1306_clear();

        qkd_init();
    }
    return ESP_OK;
}

void register_qkd_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "qkd",
        .help = "QKD Key Exchange with Dock\n",
        .hint = "[list | decrypt]",
        .func = &cmd_qkd,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

void register_qkdnoisy_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "qkd2",
        .help = "Noisy Error QKD Key Exchange with Dock\n",
        .hint = "[list | cascade | decrypt]",
        .func = &cmd_qkd2,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

void register_qkdinit_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "qkd-init",
        .help = "clear - Removes existing Quantum information\nmock - Initializes mock data without dock\n Initiate Quantum Link with Dock",
        .hint = "[clear | mock]",
        .func = &cmd_qkd_init,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}