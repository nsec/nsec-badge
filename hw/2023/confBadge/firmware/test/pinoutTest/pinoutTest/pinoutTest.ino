#include <Adafruit_NeoPixel.h>
#define PIN        9 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 8 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

void setup() 
{
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() 
{
  Serial.println(millis());
  pixels.clear(); // Set all pixel colors to 'off'
  for(int i=0; i<NUMPIXELS; i++) 
  { 
    pixels.setPixelColor(i, pixels.Color(random(0,10), random(0,10), random(0,10)));
    delay(DELAYVAL); // Pause before next pass through loop
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
}
