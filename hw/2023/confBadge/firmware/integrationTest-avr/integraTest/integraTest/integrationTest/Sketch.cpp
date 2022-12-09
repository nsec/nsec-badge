#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//NEOPIXEL DECLARATION
#define PIN			9
#define NUMPIXELS	16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//OLED DECLARATIONS
#define SCREEN_WIDTH	128 // OLED display width, in pixels
#define SCREEN_HEIGHT	32 // OLED display height, in pixels
#define OLED_RESET		-1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS	0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//NSEC COMM PORT
#define SIG_L1		2	
#define SIG_L2		3
#define SIG_L3		4
#define SIG_R1		5
#define SIG_R2		6
#define SIG_R3		7

//PROTOTYPE FUNCTION
void neopix_idle(void);
void neopix_connectLeft(void);
void neopix_connectRight(void);
void incrementLoadingBar(void);
void neopix_success(void);

//VARIABLE
bool	currentlyLoading = false;
bool	waitingForDisconnect = false;
uint8_t loadingBarPos = 0;

void setup() 
{
	//GPIO INIT
	pinMode(13, OUTPUT);
	pinMode(SIG_R2, INPUT_PULLUP);
	pinMode(SIG_R3, OUTPUT);
	digitalWrite(SIG_R3, LOW);
	
	pinMode(SIG_L2, OUTPUT);
	digitalWrite(SIG_L2, LOW);
	pinMode(SIG_L3, INPUT_PULLUP);
	
	//SERIAL INIT
	Serial.begin(9600);
  
	//NEOPIXEL INIT
	pixels.begin();

	//DISPLAY INIT
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
	{
	Serial.println(F("SSD1306 allocation failed"));
	}

	//OLED INIT
	display.setTextSize(1); // Draw 2X-scale text
	display.setTextColor(SSD1306_WHITE);
}

void loop() 
{
	//NSEC COMMUNICATION
	bool commLeftConnected = false;
	bool commRightConnected = false;
	if(digitalRead(SIG_L3) == LOW) //scan left port
		commLeftConnected = true;
	else
	{
		commLeftConnected = false;
	}
	
	if(digitalRead(SIG_R2) == LOW) //scan right port
		commRightConnected = true;
	else
	{
		commRightConnected = false;
	}
		
		
	//--------------------------------------------
	//DEBUG OUTPUT
	static uint32_t ts_uart = 0;
	if(millis() - ts_uart > 100)
	{
		ts_uart = millis();
		Serial.println(millis());	
	}
  
  
	//--------------------------------------------
	//NEOPIXEL UPDATE
	static uint32_t ts_neopix = 0;
	if(millis() - ts_neopix > 10)
	{
		ts_neopix = millis();
		if(commLeftConnected == true)
		{
			if(waitingForDisconnect == false)
			{
				incrementLoadingBar();
				neopix_connectLeft();
			}
			else
				neopix_success();		
				
		}
		else if(commRightConnected == true)
		{
			if(waitingForDisconnect == false)
			{
				neopix_connectRight();
				incrementLoadingBar();
			}
			else
				neopix_success();
		}
		else
		{
			neopix_idle();
			waitingForDisconnect = false;	//clear loading bar variables
			currentlyLoading = false;
			loadingBarPos = 0;
		}
	}

	
	//--------------------------------------------
	//DISPLAY UPDATE
	static uint32_t ts_oled = 0;
	if(millis() - ts_oled > 100)
	{
		ts_oled = millis();
		display.clearDisplay();
		display.setCursor(0,0);
		if		(commLeftConnected==true)
			display.println(F("left connected"));
		else if (commRightConnected==true)
			display.println(F("right connected"));
		else
			display.println(F("disconnected"));
		display.display();	
	}
}


//FUNCTION DECLARATION
void neopix_idle()
{
	static uint32_t ts_slowIdle = 0;
	if(millis() - ts_slowIdle > 100)	//SLOW DOWN
	{
		ts_slowIdle = millis();
		pixels.clear(); // Set all pixel colors to 'off'
		for(int i=0; i<16; i++)
		{
			pixels.setPixelColor(i, pixels.Color(0,0, random(0,10)));
		}
		pixels.show();   // Send the updated pixel colors to the hardware.	
	}
}

void neopix_success()
{
	pixels.clear(); // Set all pixel colors to 'off'
	for(int i=0; i<16; i++)
	{
		pixels.setPixelColor(i, pixels.Color(random(0,10), random(0,10), random(0,10)));
	}
	pixels.show();   // Send the updated pixel colors to the hardware.
}

void neopix_connectRight()
{
	for(int i=0; i<8; i++)
	{
		pixels.fill(pixels.Color(0,0,0), 0, 8);	//clear top row
		pixels.setPixelColor(i, pixels.Color(10,0,0) );
		pixels.show();
		delay(25);
	}	
}

void neopix_connectLeft()
{
	for(int i=0; i<8; i++)
	{
		pixels.fill(pixels.Color(0,0,0), 0, 8);	//clear top row
		pixels.setPixelColor(7-i, pixels.Color(10,0,0) );
		pixels.show();
		delay(25);
	}
}

uint32_t ts_loadingStartTime = 0;
uint32_t ts_loadingNextIncrement = 0;
void incrementLoadingBar()
{
	if(currentlyLoading == false)
	{
		currentlyLoading = true;
		ts_loadingStartTime = millis();
		ts_loadingNextIncrement = ts_loadingStartTime + 1000;
		pixels.fill( pixels.Color(0,0,0), 8, 15);	//clear bottom row
		pixels.setPixelColor(15, pixels.Color(10,0,10) );
		pixels.show();
	}
	
	if(	millis() > ts_loadingNextIncrement)
	{
		if(loadingBarPos < 8)
		{
			ts_loadingNextIncrement = millis() + 1000;	//next increment is in 1 seconds
			loadingBarPos++;
			pixels.fill( pixels.Color(0,0,0), 8, 15);	//clear bottom row
			pixels.fill( pixels.Color(10,0,10), 15-loadingBarPos+1, loadingBarPos);
			pixels.show();
		}
		else
		{
			waitingForDisconnect = true;
		}
	}
}