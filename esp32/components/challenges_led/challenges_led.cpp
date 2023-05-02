/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"

#include "FastLED.h"
#include "neopixel.h"

#include "challenges_led.h"
#include "common_led.h"

#include "challenge_led0.h"
#include "challenge_led1.h"
#include "challenge_led2.h"
#include "challenge_led3.h"

#define DATA_PIN_1 33
#define LED_TYPE WS2811

CRGB *leds;
uint16_t custom_delay = 0;

void challenges_led_init() {
    NeoPixel::getInstance().stop();
    FastLED.clear();
    FastLED.setBrightness(125);
    FastLED.show();
}

void challenges_led_end() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    NeoPixel::getInstance().start();
}

static const char *TAG = "challenges_led";

static int challenge_led(int argc, char **argv) {
    uint16_t select_challenge = 1;
    if (argc >= 2) {
        select_challenge = atoi(argv[1]);
        if(strstr(argv[1], "31337"))
            select_challenge--;
    }
    if (argc == 3) {
        custom_delay = atoi(argv[2]);
    }

    ESP_LOGI(TAG, "Running LED challenge %d!\n", select_challenge);

    challenges_led_init();
    if (select_challenge == 1)
        challenge_led1_code(custom_delay);
    else if (select_challenge == 2)
        challenge_led2_code(custom_delay);
    else if (select_challenge == 3)
        challenge_led3_code(custom_delay);
    // solve: challenge_led -34199
    else if (select_challenge == 31337)
        challenge_led0_code(custom_delay);
    challenges_led_end();
    return 0;
}

void register_challenges_led(void) {
    const esp_console_cmd_t cmd = {
        .command = "challenge_led",
        .help = "Run the LED pattern for the challenges 1,2,3\n  int16 second param: ms delay",
        .hint = "[1-3]",
        .func = &challenge_led,
        .argtable = NULL,        
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
