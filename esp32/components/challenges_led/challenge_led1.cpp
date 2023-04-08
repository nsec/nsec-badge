#include "FastLED.h"
#include "neopixel.h"
#include "esp_random.h"
#include "challenge_led1.h"
#include "common_led.h"

void led_on(CRGB *leds) {
    leds[0] = CRGB::Green;
    fill_solid( &(leds[1]), 8, CRGB::Blue);
    leds[9] = CRGB::Yellow;
    leds[10] = CRGB::Red;
    leds[11] = CRGB::Red;
    FastLED.show();
}

void led_off(CRGB *leds) {
    fill_solid( &(leds[0]), 12, CRGB::Black);
    FastLED.show();
}

int unimportant_random1(int max) {
    uint32_t rnd = esp_random();
    rnd ^= esp_random();
    return (rnd % max) + 1;
}

void challenge_led1_code(int _delay) {
    CRGB *leds;
    leds = NeoPixel::getInstance().getFastLeds();
    int DELAY = 200;
    if (_delay) DELAY = _delay;

// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'F' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'L' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'A' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'G' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'-' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'5' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'2' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'K' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'8' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'O' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'D' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'J' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'A' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'H' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'M' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'7' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'C' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity odd bits
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'H' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'K' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'Z' LSB
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);
// start bit
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// b'U' LSB
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// even parity even bits
led_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// second stop bit
led_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);
// idle state
led_on(leds);
vTaskDelay(DELAY * unimportant_random1(10) / portTICK_PERIOD_MS);

}
