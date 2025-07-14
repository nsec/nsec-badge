#include "safe.h"
#include "ssd1306.h"
#include <time.h>
#include <SmartLeds.h>

// GPIO pin definitions
#define BUTTON_OK GPIO_NUM_21
#define PIN_CLOCK     GPIO_NUM_9  // server -> client
#define PIN_S2C_DATA  GPIO_NUM_10  // server -> client
#define PIN_C2S_DATA  GPIO_NUM_11  // client -> server
#define BIT_DELAY_US  30
#define PIN_SAFE_UNLOCK GPIO_NUM_1 // GPIO pin for unlocking the safe

//LED Pins for Hardware Code Flashing
#define LED_PIN_1 GPIO_NUM_16
#define LED_PIN_2 GPIO_NUM_17
#define LED_PIN_3 GPIO_NUM_18
#define LED_PIN_4 GPIO_NUM_38
#define LED_PIN_5 GPIO_NUM_39
#define LED_PIN_6 GPIO_NUM_40
#define LED_PIN_7 GPIO_NUM_41
#define LED_PIN_8 GPIO_NUM_42

#define NUM_LEDS 8



static SmartLed led(LED_WS2812B, 1, GPIO_NUM_48, 0);

static SSD1306_t* dev = nullptr;

// Define a custom log tag for this component
static const char *TAG = "QKD-Safe";

//FLAG - "No1syQKDExchD0ne" - 16 characters
const char *FLAG_QKD_SAFE = "01001110011011110011000101110011011110010101000101001011010001000100010101111000011000110110100001000100001100000110111001100101";
const char *FLAG_ADDON = "A61729DDA4"; // 10 characters
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
        .pin_bit_mask = (1ULL << PIN_CLOCK) | (1ULL << PIN_S2C_DATA) | (1ULL << PIN_SAFE_UNLOCK) | (1ULL << LED_PIN_1) | (1ULL << LED_PIN_2) | (1ULL << LED_PIN_3) | (1ULL << LED_PIN_4) | (1ULL << LED_PIN_5) | (1ULL << LED_PIN_6) | (1ULL << LED_PIN_7) | (1ULL << LED_PIN_8),
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
    gpio_set_level(PIN_SAFE_UNLOCK, 0); // Set the safe unlock pin to low initially
}


static void binaryvalue_to_led(const std::string& binvalue)
{
    //static char PinScrambler[] = "26438715";
    //Need to use scrambled pins, not in order

    bool bitVal = (binvalue[0] == '1');
    gpio_set_level(LED_PIN_2, bitVal ? 1 : 0);

    bitVal = (binvalue[1] == '1');
    gpio_set_level(LED_PIN_6, bitVal ? 1 : 0);

    bitVal = (binvalue[2] == '1');
    gpio_set_level(LED_PIN_4, bitVal ? 1 : 0);

    bitVal = (binvalue[3] == '1');
    gpio_set_level(LED_PIN_3, bitVal ? 1 : 0);

    //This is the bit flipped pin
    bitVal = (binvalue[4] == '1');
    gpio_set_level(LED_PIN_8, bitVal ? 0 : 1); // Flipped logic for LED_PIN_8
    //gpio_set_level(LED_PIN_8, bitVal ? 1 : 0);

    bitVal = (binvalue[5] == '1');
    gpio_set_level(LED_PIN_7, bitVal ? 1 : 0);

    bitVal = (binvalue[6] == '1');
    gpio_set_level(LED_PIN_1, bitVal ? 1 : 0);

    bitVal = (binvalue[7] == '1');
    gpio_set_level(LED_PIN_5, bitVal ? 1 : 0);

    ESP_LOGI(TAG, "LEDs set to: %s", binvalue.c_str());
    return;
} 

static void blink_all(){
    binaryvalue_to_led("11111111");
    vTaskDelay(pdMS_TO_TICKS(500));
    binaryvalue_to_led("00000000");
    vTaskDelay(pdMS_TO_TICKS(500));
}

static void init_pattern(){
    blink_all();
    blink_all();
    binaryvalue_to_led("01010101");
    vTaskDelay(pdMS_TO_TICKS(500));
    binaryvalue_to_led("10101010");
    vTaskDelay(pdMS_TO_TICKS(500));
    blink_all();
    blink_all();
}

static void end_pattern(){
    blink_all();
    blink_all();
    binaryvalue_to_led("10101010");
    vTaskDelay(pdMS_TO_TICKS(500));
    binaryvalue_to_led("01010101");
    vTaskDelay(pdMS_TO_TICKS(500));
    blink_all();
    blink_all();
}

static void showtext(const std::string& displaytext)
{
    for (char ch : displaytext) {
        binaryvalue_to_led(std::bitset<8>(ch).to_string());
        vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500ms to show the LED pattern
        binaryvalue_to_led("00000000"); // Turn off LEDs after showing each character
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait for 100ms before showing the next character
    }
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

static void unlock_safe_routine(void)
{

    char flaginit[2];
    flaginit[0] = '1'; // 1 for init
    flaginit[1] = '\0'; // Null-terminate the string
    ESP_LOGI(TAG, "Send Init Data: %s", flaginit);
    server_send_bits(flaginit);  

    char flagdata[129];
    server_read_bits(flagdata, 128);
    ESP_LOGI(TAG, "Server received: %s", flagdata);

    char flagvalidate[2];
    flagvalidate[0] = (strcmp(flagdata, FLAG_QKD_SAFE) == 0) ? '1' : '0';
    flagvalidate[1] = '\0'; // Null-terminate the string
    if(flagvalidate[0] == '1') {
        ESP_LOGI(TAG, "Flag Success!: %s", flagvalidate);
        gpio_set_level(PIN_SAFE_UNLOCK, 1);
    } else {
        ESP_LOGI(TAG, "Flag Failed: %s", flagvalidate);
    }
    ESP_LOGI(TAG, "Send Validation state: %s", flagvalidate);
    server_send_bits(flagvalidate);

    ESP_LOGI(TAG, "Safe Unlock Routine Completed");
    vTaskDelay(pdMS_TO_TICKS(10000)); //Leave unlocked for 10 seconds
}

void button_task_safe(void *param)
{

    while(true)
    {
        led[0] = Rgb{122, 0, 0};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Waiting for badge to be docked (GPIO21 goes HIGH)...");
        // 1) Wait until the pin goes HIGH (badge connected/powered)
        while (gpio_get_level(BUTTON_OK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        led[0] = Rgb{0, 122, 0};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Badge detected! Now waiting for button press (GPIO21 goes LOW)...");
        // 2) Now wait for a LOW, meaning the user pressed the button
        while (gpio_get_level(BUTTON_OK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        led[0] = Rgb{0, 0, 122};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Button press detected! Proceeding with Unlock Safe workflow...");
        // Debounce
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        
        //first display the flag on the addon:
        init_pattern();
        showtext(FLAG_ADDON);
        end_pattern();
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait for 100 ms before proceeding

        //then, do the unlock routine
        unlock_safe_routine();

        vTaskDelay(pdMS_TO_TICKS(5*60000)); // Wait for 5 minutes before checking again
    }

    xSemaphoreGive(task_complete_semaphore);
    vTaskDelete(NULL);  // Delete the task once done
}

int cmd_safe(int argc __attribute__((unused)),char **argv __attribute__((unused)))
{
        printf("\n--- Unlock Safe Workflow ---\n");
        // Initialize the semaphore if not already created
        if (task_complete_semaphore == NULL) {
            task_complete_semaphore = xSemaphoreCreateBinary();
        }
        buttons_init();
        bus_init();
        
        xTaskCreate(button_task_safe, "button_task_safe", 4096, NULL, 1, NULL);
        xSemaphoreTake(task_complete_semaphore, portMAX_DELAY);
    return ESP_OK;
}

void register_safe_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "safe",
        .help = "Initiate QKD Key Exchange with chip\n",
        .hint = "",
        .func = &cmd_safe,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}