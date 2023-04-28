#include "FastLED.h"
#include "neopixel.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "challenge_led1.h"
#include "common_led.h"

esp_timer_handle_t hop_timer;

uint8_t hop_bits[] = {

// F LSB
0x00000062,
0x00000000,

// L LSB
0x00000032,
0x00000000,

// A LSB
0x00000082,
0x00000000,

// G LSB
0x000000e2,
0x00000000,

// - LSB
0x000000b4,
0x00000000,

// Z LSB
0x0000005a,
0x00000000,

// 9 LSB
0x0000009c,
0x00000000,

// S LSB
0x000000ca,
0x00000000,

// Q LSB
0x0000008a,
0x00000000,

// F LSB
0x00000062,
0x00000000,

// P LSB
0x0000000a,
0x00000000,

// F LSB
0x00000062,
0x00000000,

// B LSB
0x00000042,
0x00000000,

// 3 LSB
0x000000cc,
0x00000000,

// P LSB
0x0000000a,
0x00000000,

// 5 LSB
0x000000ac,
0x00000000,

// 4 LSB
0x0000002c,
0x00000000,

// Y LSB
0x0000009a,
0x00000000,

// 0 LSB
0x0000000c,
0x00000000,

// B LSB
0x00000042,
0x00000000,

// 4 LSB
0x0000002c,
0x00000000,


};
uint32_t hop_pt = 0;
int hop_delay = 30000 * 6;

void led_blues() {
    fill_solid( &(leds[0]), 2, CRGB::Blue);
    fill_solid( &(leds[2]), 8, CRGB::Black);
    fill_solid( &(leds[10]), 2, CRGB::Blue);
    FastLED.show();
}

int unimportant_random0(int max) {
    uint32_t rnd = esp_random();
    rnd ^= esp_random();
    return 500;
    //return (rnd % max) + 2501;
}

static void challenge_led0_timer_cycle(void *arg) {
    //ESP_LOGI("test", "hop cycle, time since boot: %lld us", esp_timer_get_time());
    //ESP_LOGI("test", "%#10x", hop_bits[hop_pt]);
    
    for (int i=2; i < 10; i++) {
        leds[i] = (hop_bits[hop_pt] & (1<<(i-2)) ? CRGB::MediumAquamarine : CRGB::MediumVioletRed);
    }
    FastLED.show();

    if (++hop_pt >= sizeof(hop_bits)) {
        hop_pt = 0;
        esp_timer_stop(hop_timer);
        vTaskDelay(hop_delay + 100 / portTICK_PERIOD_MS);
        led_blues();
    }
}

void challenge_led0_code(int _delay) {
    leds = NeoPixel::getInstance().getFastLeds();
    if (_delay) hop_delay = _delay;

    led_blues();

    vTaskDelay(unimportant_random0(2500) / portTICK_PERIOD_MS);

    const esp_timer_create_args_t hop_timer_args = { .callback = challenge_led0_timer_cycle };
    esp_timer_create(&hop_timer_args, &hop_timer);
    esp_timer_start_periodic(hop_timer, hop_delay * 1000);
    //ESP_LOGI("test", "Started timers, time since boot: %lld us", esp_timer_get_time());
    while(esp_timer_is_active(hop_timer)) {
        vTaskDelay(unimportant_random0(2500) / portTICK_PERIOD_MS);
    }
    esp_timer_delete(hop_timer);

}
