/*
 * oledControl.cpp
 *
 * Created: 2022-12-20 2:41:04 PM
 *  Author: lambert
 */ 

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "uidFetcher.h"
#include "cypherControl.h"

//OLED DECLARATIONS
#define SCREEN_WIDTH	128 // OLED display width, in pixels
#define SCREEN_HEIGHT	32 // OLED display height, in pixels
#define OLED_RESET		-1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS	0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void oled_init()
{
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		Serial.println(F("SSD1306 allocation failed"));
	}

	//OLED INIT
	display.setTextColor(SSD1306_WHITE);
	display.clearDisplay();
	display.setTextSize(1);
	display.println("Northsec2023");
	UniqueIDdump(display);
	display.display();      // Show initial text
}

void oled_outputUID()
{
	display.clearDisplay();
	display.setCursor(0,0);
	for (size_t i = 0; i < 8; i++)
	{
		if (UniqueID8[i] < 0x10)
		display.print("0");
		display.print(UniqueID8[i], HEX);
		display.print("");
	}
	display.display();      // Show initial text
}

/*
void oled_testCypher()
{
	display.clearDisplay();
	display.setCursor(0,0);
	
	//generate key
	uint32_t test_key =	cypher_getKey();
	display.println(test_key);
	uint8_t arrayGenKeyFactors[4];
	cypher_getGeneratedMultipliers(arrayGenKeyFactors);
	for(int i=0; i<4; i++)
	{
		display.print(arrayGenKeyFactors[i]);
		display.print(" ");
	}
	display.println();
	
	//validate if key is valid
	display.println(cypher_checkIfValidKey(test_key));
	
	//determine which factors were used
	uint8_t arrayValidatedKeyFactors[4];
	cypher_getValidatedMultipliers(arrayValidatedKeyFactors);
	for(int i=0; i<4; i++)
	{
		display.print(arrayValidatedKeyFactors[i]);
		display.print(" ");
	}
	display.display();
}
*/

void oled_showSoftSerial()
{
	display.clearDisplay();
	display.setCursor(0,0);
	display.print("broadcast:");
	//display.println(sending);
	display.print("\nLeft RX:");
	//display.println(receivedLeft);
	display.print("Right RX:");
	//display.println(receivedRight);
	display.display();
}

void oled_showHackLevel()
{
	/*
	display.clearDisplay();
	display.setCursor(0,0);
	display.println();
	display.print(F("LVL="));
	display.print(level_getCurrLvl());
	display.display();
	*/
}