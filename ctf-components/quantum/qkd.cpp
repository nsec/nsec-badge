#include "qkd.h"

#define PIN_CLOCK     GPIO_NUM_2  // input (server -> client)
#define PIN_S2C_DATA  GPIO_NUM_7  // input (server -> client)
#define PIN_C2S_DATA  GPIO_NUM_6  // output (client -> server)

#define BIT_DELAY_US  50

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

static void test_ping_pong(void)
{
    // 1) Read 4 bits from the server
    char pingBuf[5];
    client_read_bits(pingBuf, 4);
    // e.g. might be "1010"
    printf("Client received: %s\n", pingBuf);

    // 2) Then send back 4 bits
    const char *pongMsg = "0101";
    printf("Client sending: %s\n", pongMsg);
    client_send_bits(pongMsg);
}

int cmd_qkd(int argc, char **argv)
{

    if (strcmp(argv[1], "init") == 0) {
        printf("\n--- Init QKD WorkFlow ---\n");
        bus_init();
        test_ping_pong();
    }
    else if (strcmp(argv[1], "decrypt") == 0) {
        printf("\n--- Decrypt QKD WorkFlow ---\n");
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
        .hint = "init | decrypt",
        .func = &cmd_qkd,
        .argtable = NULL,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}