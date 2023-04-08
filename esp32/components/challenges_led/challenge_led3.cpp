#include "FastLED.h"
#include "neopixel.h"
#include "challenge_led3.h"
#include "common_led.h"

void led1_on(CRGB *leds) {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    FastLED.show();
}

void led1_off(CRGB *leds) {
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Red;
    FastLED.show();
}

void led2_on(CRGB *leds) {
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    FastLED.show();
}

void led2_off(CRGB *leds) {
    leds[2] = CRGB::Red;
    leds[3] = CRGB::Red;
    FastLED.show();
}

void led3_on(CRGB *leds) {
    leds[8] = CRGB::Green;
    leds[9] = CRGB::Green;
    FastLED.show();
}

void led3_off(CRGB *leds) {
    leds[8] = CRGB::Red;
    leds[9] = CRGB::Red;
    FastLED.show();
}

void led4_on(CRGB *leds) {
    leds[10] = CRGB::Green;
    leds[11] = CRGB::Green;
    FastLED.show();
}

void led4_off(CRGB *leds) {
    leds[10] = CRGB::Red;
    leds[11] = CRGB::Red;
    FastLED.show();
}

void challenge_led3_code(int _delay) {
    CRGB *leds;
    leds = NeoPixel::getInstance().getFastLeds();
    int DELAY = 100;
    if (_delay) DELAY = _delay;

// idle state
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// SCL_IN VOH + sleep
led4_on(leds);
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'F' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'L' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'A' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'G' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'-' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'1' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'5' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'Q' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'M' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'8' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'W' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'A' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'Z' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'B' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'U' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'S' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'N' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'V' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'C' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'P' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'C' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'B' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'I' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'N' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'E' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'7' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'F' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'H' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'F' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'I' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'X' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'R' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// start bit
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// address frame
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// r/w bit send=0, receive=1
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// b'U' MSB
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// ACK succesful
// SDA_IN VOL
led3_off(leds);
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SCL VOL + sleep
led2_off(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// stop bit
// SDA VOL
led1_off(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA VOH
led1_on(leds);

// idle state
// SDA VOH
led1_on(leds);

// SCL VOH + sleep
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

// SDA_IN VOH
led3_on(leds);
led1_on(leds);

// SCL_IN VOH + sleep
led4_on(leds);
led2_on(leds);
vTaskDelay(DELAY / portTICK_PERIOD_MS);

}
