#include "FastLED.h"
#include "neopixel.h"
#include "esp_random.h"
#include "challenge_led2.h"
#include "common_led.h"

void led1(bool b) {
    if (b) {
        leds[0] = CRGB::Blue;
        leds[1] = CRGB::Blue;
        leds[2] = CRGB::Blue;
    } else {
        leds[0] = CRGB::Orange;
        leds[1] = CRGB::Orange;
        leds[2] = CRGB::Orange;
    }
    FastLED.show();
}

void led2(bool b) {
    if (b) {
        leds[3] = CRGB::Blue;
        leds[4] = CRGB::Blue;
        leds[5] = CRGB::Blue;
    } else {
        leds[3] = CRGB::Orange;
        leds[4] = CRGB::Orange;
        leds[5] = CRGB::Orange;
    }
    FastLED.show();
}

void led3(bool b) {
    if (b) {
        leds[6] = CRGB::Blue;
        leds[7] = CRGB::Blue;
        leds[8] = CRGB::Blue;
    } else {
        leds[6] = CRGB::Orange;
        leds[7] = CRGB::Orange;
        leds[8] = CRGB::Orange;
    }
    FastLED.show();
}

void led4(bool b) {
    if (b) {
        leds[9] = CRGB::Blue;
        leds[10] = CRGB::Blue;
        leds[11] = CRGB::Blue;
    } else {
        leds[9] = CRGB::Orange;
        leds[10] = CRGB::Orange;
        leds[11] = CRGB::Orange;
    }
    FastLED.show();
}

void led5(bool b) {
    if (b) {
        leds[12] = CRGB::Blue;
        leds[13] = CRGB::Blue;
        leds[14] = CRGB::Blue;
    } else {
        leds[12] = CRGB::Orange;
        leds[13] = CRGB::Orange;
        leds[14] = CRGB::Orange;
    }
    leds[15] = CRGB::Black;
    leds[16] = CRGB::Black;
    FastLED.show();
}

int unimportant_random2(int max) {
    uint32_t rnd = esp_random();
    rnd ^= esp_random();
    return (rnd % max) + 1 + 2000;
}

void challenge_led2_code(int _delay) {
    leds = NeoPixel::getInstance().getFastLeds();
    int DELAY = 60;
    if (_delay) DELAY = _delay;

// idle state CKP=0
led1(0);
led2(1);
led3(1);
led4(1);
led5(1);
vTaskDelay(unimportant_random2(2000) / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'q' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'q' miso: b'F'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'{' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'{' miso: b'L'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'v' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'v' miso: b'A'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'p' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'p' miso: b'G'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x1a' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x1a' miso: b'-'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x06' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x06' miso: b'1'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x03' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x03' miso: b'4'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'p' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'p' miso: b'G'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'g' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'g' miso: b'P'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x06' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x06' miso: b'1'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'p' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'p' miso: b'G'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x03' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x03' miso: b'4'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x00' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x00' miso: b'7'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'f' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'f' miso: b'Q'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'q' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'q' miso: b'F'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'~' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'~' miso: b'I'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'z' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'z' miso: b'M'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'a' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'a' miso: b'V'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x00' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x00' miso: b'7'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x7f' MSB
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x7f' miso: b'H'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOL
led4(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'<' MSB
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'\x06' MSB
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS1 VOH
led4(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOL
led5(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// b'^' MSB
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// mosi: b'\x06' miso: b'1'  (MSB)
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
led2(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
led3(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOH
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MOSI VOL
led2(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOL
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// MISO VOH
led3(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// SCLK
led1(1);
// CS2 VOH
led5(1);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
led1(0);
vTaskDelay(DELAY/2 / portTICK_PERIOD_MS);
// idle state CKP=0
led1(0);
led2(1);
led3(1);
led4(1);
led5(1);
vTaskDelay(unimportant_random2(2000) / portTICK_PERIOD_MS);

}
