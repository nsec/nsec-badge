/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "challenges_led.h"
#include "esp_console.h"
#include "esp_log.h"

#include "FastLED.h"
#include "neopixel.h"

#include "challenge_led1.h"

#define DATA_PIN_1 33
#define LED_TYPE WS2811

void challenges_led_init() {
    NeoPixel::getInstance().stop();
    FastLED.clear();
    FastLED.setBrightness(25);
}

void challenges_led_end() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    NeoPixel::getInstance().start();
}


// TODO move this to updated cmd.cpp
static const char *TAG = "challenges_led";

void register_challenges_led(void)
{
    register_led1();
    //register_led2();
}

static int challenge_led1(int argc, char **argv) {
    ESP_LOGI(TAG, "This is LED challenge 1!\n");
    challenges_led_init();
    challenge_led1_code();
    challenges_led_end();
    return 0;
}

void register_led1(void) {
    const esp_console_cmd_t cmd = {
        .command = "challenge_led1",
        .help = "Run the LED pattern for the challenge 1",
        .hint = NULL,
        .func = &challenge_led1,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
