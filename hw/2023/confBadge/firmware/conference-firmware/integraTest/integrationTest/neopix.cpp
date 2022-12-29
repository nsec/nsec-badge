/*
 * neopix.cpp
 *
 * Created: 2022-12-09 2:21:41 AM
 *  Author: Lam
 */ 
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "levelControl.h"
#include "modeControl.h"

//NEOPIXEL DECLARATION
#define PIN			9
#define NUMPIXELS	16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t idle_refreshInterval = 200;
uint8_t loadingBarPos = 0;


void neopix_init()
{
	pixels.begin();
}

void neopix_idle()
{
	static uint32_t ts_slowIdle = 0;
	static uint8_t startPosition = 0;
	if(millis() - ts_slowIdle > idle_refreshInterval)	//slow down animation
	{
		ts_slowIdle = millis();
		startPosition++;
		
		if(startPosition > 15)	//wrap around when reaching the end led strip
			startPosition = 0;	
			
		pixels.clear(); // Set all pixel colors to 'off'
		for(int i=0; i<level_getCurrLvl(); i++)		//determine how many LED should be one
		{
			//led_ID is the current LED index that we are update. 
			uint8_t led_ID = (startPosition+i) % 16; //This is going over every single LED that needs to be on based on the current LVL
			
			/*
			if(led_ID > 15)
				led_ID = i;	//wrap around
			*/
				
			//enable more colors if your lvl is more than the 16 LEDs
			uint8_t b = 0;
			uint8_t r = 0;
			uint8_t g = 0;
			uint8_t r_factor = 1;
			uint8_t g_factor = 1;
			uint8_t b_factor = 1;
			if (i < 16)			//only blue to start
			{
				b = 5;
			}
			else if(i >= 16 && i < 32)	//introduce red 
			{
				b = 5;
				r = 5;
			}
			else if(i >= 32 && i < 48)	//introduce green
			{
				b = 5;
				g = 5;	
			}
			else if(i >= 48 && i < 64)	//have all 3 colors
			{
				g=5;	
				r=5;
				b=5;
			}
			else if(i >= 64 && i < 80)	//increase brightness + speed
			{
				idle_refreshInterval = 100;
				b=10;
				r=5;
				b=5;
			}
			else if(i >= 80 && i < 96)	//increase brightness
			{
				idle_refreshInterval = 50;
				b=10;
				r=10;
				b=5;
			}
			else if(i >= 96 && i < 112)	//increase brightness
			{
				idle_refreshInterval = 25;
				b=10;
				r=10;
				b=10;
			}
			else if(i >= 112 && i < 128)	//max score
			{
				idle_refreshInterval = 12;
				r=random(1,10);
				g=0;
				b=0;
			}
			else if(i >= 128 && i < 144)	//max score
			{
				idle_refreshInterval = 6;
				r=random(1,10);
				g=0;				
				b=random(1,10);
			}
			else if(i >= 144 && i < 160)	//max score
			{
				idle_refreshInterval = 3;
				r=random(1,10);
				g=random(1,10);
				b=random(1,10);
			}
			else if(i >= 160)	//max score
			{
				idle_refreshInterval = 1;
				r=random(1,15);
				g=random(1,15);
				b=random(1,15);
			}

				
			//apply color
			pixels.setPixelColor(led_ID, pixels.Color(r,g,b)	);	
		}
		pixels.show();   // Send the updated pixel colors to the hardware.	
	}
}

void neopix_success()
{
	static uint32_t ts_lastRefresh = 0;
	
	if(millis() - ts_lastRefresh > 10)
	{
		ts_lastRefresh = millis();
		pixels.clear(); // Set all pixel colors to 'off'
		for(int i=0; i<16; i++)
		{
			pixels.setPixelColor(i, pixels.Color(random(0,10), random(0,10), random(0,10)));
		}
		pixels.show();   // Send the updated pixel colors to the hardware.
	}
}

void neopix_connectRight()
{
	static uint32_t ts_lastRefresh = 0;
	static int i = 0;
	
	if(millis() - ts_lastRefresh > 25)
	{
		ts_lastRefresh = millis();
		
		i++;
		if(i==7)
			i=0;
			
		pixels.fill(pixels.Color(0,0,0), 0, 8);	//clear top row
		pixels.setPixelColor(i, pixels.Color(10,0,0) );
		pixels.show();
	}
}

void neopix_connectLeft()
{
	static uint32_t ts_lastRefresh = 0;
	static int i = 0;
	
	if(millis() - ts_lastRefresh > 25)
	{
		ts_lastRefresh = millis();
			
		i++;
		if(i==7)
			i=0;
			
		pixels.fill(pixels.Color(0,0,0), 0, 8);	//clear top row
		pixels.setPixelColor(7-i, pixels.Color(10,0,0) );
		pixels.show();
	}
}

uint32_t ts_loadingStartTime = 0;
uint32_t ts_loadingNextIncrement = 0;
void incrementLoadingBar()
{
	if(mode_getCurrentlyLoading() == false)
	{
		mode_currentLoading_assert();
		ts_loadingStartTime = millis();
		ts_loadingNextIncrement = ts_loadingStartTime + 1000;
		pixels.fill( pixels.Color(0,0,0), 8, 15);	//clear bottom row
		pixels.setPixelColor(15, pixels.Color(30,0,30) );
		pixels.show();
	}
	
	if(	millis() > ts_loadingNextIncrement && mode_getWaitForDisconnect() == false)
	{
		if(loadingBarPos < 8)
		{
			ts_loadingNextIncrement = millis() + 1000;	//next increment is in 1 seconds
			loadingBarPos++;
			pixels.fill( pixels.Color(0,0,0), 8, 15);	//clear bottom row
			pixels.fill( pixels.Color(30,0,30), 15-loadingBarPos+1, loadingBarPos);
			pixels.show();
		}
		else
		{			
			
			if(level_getCurrLvl()<199)	
				level_increment();		//cap level at 199. No more additional LED animation after 199
			mode_waitForDisconnect_assert();
		}
	}
}

void neopix_resetLoadingBar()
{
	loadingBarPos = 0;
}