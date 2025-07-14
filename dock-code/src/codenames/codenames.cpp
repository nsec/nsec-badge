#include "codenames.h"
#include "ssd1306.h"
#include <time.h>
#include <SmartLeds.h>

// GPIO pin definitions
#define BUTTON_OK GPIO_NUM_21
#define PIN_CLOCK     GPIO_NUM_9  // server -> client
#define PIN_S2C_DATA  GPIO_NUM_10  // server -> client
#define PIN_C2S_DATA  GPIO_NUM_11  // client -> server
#define BIT_DELAY_US  30

static const char *TAG = "DOCK-RF";
static SmartLed led(LED_WS2812B, 1, GPIO_NUM_48, 0);
static SemaphoreHandle_t task_complete_semaphore = NULL;

static const char *RF_VALID = "1011101101111111011101111"; // 25 bits

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

static bool validated(const char *rfdata)
{
    // Example validation logic
    // Loop through RF_VALID and compare with rfdata
    // If all bits match, return true, otherwise return false
    for (size_t i = 0; i < strlen(RF_VALID); i++)
    {
        //ESP_LOGI(TAG, "Validation step %zu: expected %c, got %c", i, RF_VALID[i], rfdata[i]);
        if (RF_VALID[i] != rfdata[i])
        {
            ESP_LOGI(TAG, "Validate Step: %zu", i); // Corrected to use %zu for size_t
            return false; // Validation failed
        }
    }
    return true;
}

static void rf_routine(void)
{
    char flaginit[2];
    flaginit[0] = '1'; // 1 for init
    flaginit[1] = '\0'; // Null-terminate the string
    ESP_LOGI(TAG, "Send Init Data: %s", flaginit);
    server_send_bits(flaginit);  

    char rfdata[26];
    server_read_bits(rfdata, 25);
    ESP_LOGI(TAG, "Server received: %s", rfdata);

    //TODO - validation routine

    if(validated(rfdata)) {
        ESP_LOGI(TAG, "Validate Success!: %s", rfdata);
        led[0] = Rgb{0, 122, 0};
        led.show();
        led.wait();
        
    } else {
        ESP_LOGI(TAG, "Validate Failed: %s", rfdata);
        led[0] = Rgb{122, 0, 0};
        led.show();
        led.wait();
    }
   
    ESP_LOGI(TAG, "RF Validate Routine Completed");
}

void button_task_rf(void *param)
{

    while(true)
    {
        led[0] = Rgb{0, 0, 122};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Waiting for badge to be docked (GPIO21 goes HIGH)...");
        // 1) Wait until the pin goes HIGH (badge connected/powered)
        while (gpio_get_level(BUTTON_OK) == 0) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        ESP_LOGI(TAG, "Badge detected! Now waiting for button press (GPIO21 goes LOW)...");
        // 2) Now wait for a LOW, meaning the user pressed the button
        while (gpio_get_level(BUTTON_OK) == 1) {
            vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        }

        //essentially, blink
        led[0] = Rgb{0, 0, 0};
        led.show();
        led.wait();

        vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 0.5 second

        led[0] = Rgb{0, 0, 122};
        led.show();
        led.wait();

        ESP_LOGI(TAG, "Button press detected! Proceeding with RF Validate...");
        // Debounce
        vTaskDelay(pdMS_TO_TICKS(BIT_DELAY_US));
        
        rf_routine();

        vTaskDelay(pdMS_TO_TICKS(20000)); // Wait for 20 seconds before checking again
    }

    xSemaphoreGive(task_complete_semaphore);
    vTaskDelete(NULL);  // Delete the task once done
}

int cmd_rf(int argc __attribute__((unused)),char **argv __attribute__((unused)))
{
        printf("\n--- RF Validate Workflow ---\n");
        // Initialize the semaphore if not already created
        if (task_complete_semaphore == NULL) {
            task_complete_semaphore = xSemaphoreCreateBinary();
        }
        buttons_init();
        bus_init();
        
        xTaskCreate(button_task_rf, "button_task_rf", 4096, NULL, 1, NULL);
        xSemaphoreTake(task_complete_semaphore, portMAX_DELAY);
    return ESP_OK;
}

void register_safe_cmd() {
    const esp_console_cmd_t cmd = {
        .command = "rf",
        .help = "Initiate RF validate with chip\n",
        .hint = "",
        .func = &cmd_rf,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}