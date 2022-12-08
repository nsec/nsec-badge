/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <Adafruit_NeoPixel.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


#define PIN        9 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 16 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 20 // Time (in milliseconds) to pause between pixels

bool ledState = false;
void setup() 
{
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	pinMode(A6, INPUT_PULLUP);
	pinMode(A7, INPUT_PULLUP);
	pinMode(13, OUTPUT);
	digitalWrite(13, ledState);
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}


void loop() 
{
	ledState = !ledState;
	digitalWrite(13, ledState);
	
	static uint32_t ts_uart = 0;
	if(millis() - ts_uart > 250)
	{
		ts_uart = millis();
	  Serial.print(millis()); Serial.print("\t"); 
	  Serial.print(digitalRead(A0)); Serial.print("\t"); 
	  Serial.print(digitalRead(A1)); Serial.print("\t"); 
	  Serial.print(digitalRead(A2)); Serial.print("\t"); 
	  Serial.print(digitalRead(A3)); Serial.print("\t"); 
	  Serial.print(digitalRead(A6)); Serial.print("\t"); 
	  Serial.print(digitalRead(A7)); Serial.println("\t"); 
	}
  
  pixels.clear(); // Set all pixel colors to 'off'
  for(int i=0; i<NUMPIXELS; i++) 
  { 
    pixels.setPixelColor(i, pixels.Color(random(0,10), random(0,10), random(0,10)));
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(DELAYVAL); // Pause before next pass through loop
}
