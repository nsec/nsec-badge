#include "qkd.h"
#include "ssd1306.h"
#include <time.h>
#include <SmartLeds.h>

// GPIO pin definitions
#define BUTTON_OK GPIO_NUM_21
#define PIN_CLOCK     GPIO_NUM_9  // server -> client
#define PIN_S2C_DATA  GPIO_NUM_10  // server -> client
#define PIN_C2S_DATA  GPIO_NUM_11  // client -> server
#define BIT_DELAY_US  30

static SSD1306_t* dev = nullptr;

#define SDA_PIN 47
#define SCL_PIN 48

static SmartLed led(LED_WS2812B, 1, GPIO_NUM_48, 0);

// Define a custom log tag for this component
static const char *TAG = "QKD";

const char *FLAG_QKD = "QKDHeistSecuR3D!"; //16 characters
const char *FLAG_QKD2 = "No1syQKDExchD0ne"; //16 characters

// Semaphore to block console until task finishes
static SemaphoreHandle_t task_complete_semaphore = NULL;

static void buttons_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_OK),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

static void bus_init(void)
{
    // CLOCK + S2C_DATA as outputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_CLOCK) | (1ULL << PIN_S2C_DATA),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // C2S_DATA as input
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_C2S_DATA);
    gpio_config(&io_conf);

    // Default low
    gpio_set_level(PIN_CLOCK, 0);
    gpio_set_level(PIN_S2C_DATA, 0);
}

// Function to convert string to binary representation
std::string string_to_binary(const char* input) {
    std::string binary_representation;
    for (int i = 0; input[i] != '\0'; ++i) {
        std::bitset<8> binary(input[i]);  // Convert each character to 8-bit binary
        binary_representation += binary.to_string();
    }
    return binary_representation;
}

// Function to convert std::string to char*
char* string_to_char_pointer(const std::string& str) {
    char* c_str = new char[str.length() + 1];  // Allocate memory for the char array
    strcpy(c_str, str.c_str());           // Copy the string data
    return c_str;                              // Return the pointer
}

static void server_send_bits(const char *bits)
{
    size_t len = strlen(bits);
    for (size_t i = 0; i < len; i++)
    {
        bool bitVal = (bits[i] == '1');
        gpio_set_level(PIN_S2C_DATA, bitVal ? 1 : 0);

        // small delay to let data settle
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));

        // Toggle clock HIGH
        gpio_set_level(PIN_CLOCK, 1);
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));

        // Clock LOW
        gpio_set_level(PIN_CLOCK, 0);
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
    }
    // Optionally set data line low
    gpio_set_level(PIN_S2C_DATA, 0);
}

static void server_read_bits(char *outBuf, size_t numBits)
{
    for (size_t i = 0; i < numBits; i++)
    {
        // small delay if needed
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));

        // Clock HIGH -> sample
        gpio_set_level(PIN_CLOCK, 1);
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));

        int bitVal = gpio_get_level(PIN_C2S_DATA);

        // Clock LOW
        gpio_set_level(PIN_CLOCK, 0);
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));

        outBuf[i] = bitVal ? '1' : '0';
    }
    outBuf[numBits] = '\0';
}

void generate_random_bit_string(char *buffer, int length) {
    srand(time(NULL));  // Seed the random number generator
    for (int i = 0; i < length; ++i) {
        buffer[i] = (rand() % 2) ? '1' : '0';  // Add '0' or '1' to the buffer
    }
    buffer[length] = '\0';  // Null-terminate the string
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
            shared_key[j] = key[i];
            j++;
        }
    }

    // Null-terminate the shared key string
    shared_key[j] = '\0';
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

// Function to convert a string to a binary string representation
void string_to_binary(const char *input, char *output) {
    output[0] = '\0';  // Start with an empty string

    for (size_t i = 0; i < strlen(input); ++i) {
        char binary[9];
        for (int bit = 7; bit >= 0; --bit) {
            binary[7 - bit] = ((input[i] >> bit) & 1) ? '1' : '0';
        }
        binary[8] = '\0';
        strcat(output, binary);
    }
}

// Function to XOR two binary strings, wrapping the key if needed
void xor_binary_with_wrapped_key(const char *bin_flag, const char *bin_key, char *output) {
    size_t flag_len = strlen(bin_flag);
    size_t key_len = strlen(bin_key);

    for (size_t i = 0; i < flag_len; ++i) {
        // Wrap the key using modulo indexing
        output[i] = (bin_flag[i] == bin_key[i % key_len]) ? '0' : '1';
    }
    output[flag_len] = '\0';  // Null-terminate the output string
}

static void qkd_routine(void)
{
    char dockBits[129];
    generate_random_bit_string(dockBits, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Send dockkey: %s", dockBits);
    server_send_bits(dockBits);  

    char badgeBasis[129];
    server_read_bits(badgeBasis, 128);
    ESP_LOGI(TAG, "Server received: %s", badgeBasis);

    char dockBasis[129];
    generate_random_bit_string(dockBasis, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Send dockbasis: %s", dockBasis);
    server_send_bits(dockBasis);

    char dockKey[129];
    generate_random_bit_string(dockKey, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    server_send_bits(dockKey);
    ESP_LOGI(TAG, "Send dockkey: %s", dockKey);

    char shared_key[129];
    generate_key_from_basis(dockKey, badgeBasis, dockBasis, shared_key);
    char binary_flag[129];
    string_to_binary(FLAG_QKD, binary_flag);
    ESP_LOGI(TAG, "BINARY FLAG: %s", binary_flag);
    ESP_LOGI(TAG, "XOR KEY: %s", shared_key);
    char xor_result[129];
    xor_binary_with_wrapped_key(binary_flag, shared_key, xor_result);

    //FLAG_QKD
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Send ciphertext: %s", xor_result);
    server_send_bits(xor_result);



    /* ------- Part Two of noisy based qkd2 routine -------- */

    char dockBits2[129];
    generate_random_bit_string(dockBits2, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    server_send_bits(dockBits2);
    ESP_LOGI(TAG, "Send dockkey2: %s", dockBits2);  

    char badgeBasis2[129];
    server_read_bits(badgeBasis2, 128);
    ESP_LOGI(TAG, "Server received: %s", badgeBasis2);

    char dockBasis2[129];
    generate_random_bit_string(dockBasis2, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Send dockbasis2: %s", dockBasis2);
    server_send_bits(dockBasis2);

    char dockKey2[129];
    generate_random_bit_string(dockKey2, 128);
    vTaskDelay(pdMS_TO_TICKS(3000));
    server_send_bits(dockKey2);
    ESP_LOGI(TAG, "Send dockkey2: %s", dockKey2);

    char shared_key2[129];
    generate_key_from_basis(dockKey2, badgeBasis2, dockBasis2, shared_key2);
    char binary_flag2[129];
    string_to_binary(FLAG_QKD2, binary_flag2);
    ESP_LOGI(TAG, "BINARY FLAG2: %s", binary_flag2);
    ESP_LOGI(TAG, "XOR KEY2: %s", shared_key2);
    char xor_result2[129];
    xor_binary_with_wrapped_key(binary_flag2, shared_key2, xor_result2);

    //FLAG_QKD

    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Send ciphertext2: %s", xor_result2);
    server_send_bits(xor_result2);
    
    //QKD Completed
    ESP_LOGI(TAG, "QKD Routine Completed");
    vTaskDelay(pdMS_TO_TICKS(20000));
}

void button_task(void *param)
{

    while(true){
        
        led[0] = Rgb{122, 0, 0};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Waiting for badge to be docked (GPIO21 goes HIGH)...");
        //badge_ssd1306_clear();
        //badge_print_text(0, "Wait for Badge", 14, false);
        // 1) Wait until the pin goes HIGH (badge connected/powered)
        while (gpio_get_level(BUTTON_OK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        led[0] = Rgb{0, 122, 0};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Badge detected! Now waiting for button press (GPIO21 goes LOW)...");
        //badge_print_text(1, "Badge detected!", 15, false);
        // 2) Now wait for a LOW, meaning the user pressed the button
        while (gpio_get_level(BUTTON_OK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        led[0] = Rgb{0, 0, 122};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Button press detected! Proceeding with QKD routine...");
        //badge_print_text(2, "Button pressed", 14, false);
        //badge_print_text(3, "Start Transfer..", 16, false);
        // Debounce
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        
        //test_ping_pong();
        qkd_routine();
    }

    xSemaphoreGive(task_complete_semaphore);
    vTaskDelete(NULL);  // Delete the task once done
}

int cmd_qkd(int argc __attribute__((unused)),char **argv __attribute__((unused)))
{
        printf("\n--- QKD WorkFlow ---\n");
        // Initialize the semaphore if not already created
        if (task_complete_semaphore == NULL) {
            task_complete_semaphore = xSemaphoreCreateBinary();
        }
        buttons_init();
        bus_init();
        //badge_ssd1306_init();
        
        xTaskCreate(button_task, "button_task", 4096, NULL, 1, NULL);
        xSemaphoreTake(task_complete_semaphore, portMAX_DELAY);
    return ESP_OK;
}

void register_qkd_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "qkd",
        .help = "Initiate QKD Key Exchange with chip\n",
        .hint = "",
        .func = &cmd_qkd,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}