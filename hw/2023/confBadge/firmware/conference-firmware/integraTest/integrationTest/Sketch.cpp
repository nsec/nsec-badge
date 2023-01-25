#include <Arduino.h>
#include "neopix.h"
#include "levelControl.h"
#include "modecontrol.h"
#include "softserialControl.h"
#include "uidFetcher.h"
#include "oledControl.h"
#include "cypherControl.h"

//BUTTON
#define BTN_U	A1
#define BTN_D	A2
#define BTN_L	A3
#define BTN_R	A0
#define BTN_A	A6
#define BTN_B	A7

//NSEC COMM PORT
#define SIG_L1		2	
#define SIG_L2		3
#define SIG_L3		4
#define SIG_R1		5
#define SIG_R2		6
#define SIG_R3		7

//PROTOTYPE FUNCTION
void display_idle(void);


void setup() 
{
	//GPIO INIT
	pinMode(13, OUTPUT);
	pinMode(BTN_U, INPUT);
	pinMode(BTN_D, INPUT);
	pinMode(BTN_R, INPUT);
	pinMode(BTN_L, INPUT);
	pinMode(BTN_A, INPUT);
	pinMode(BTN_B, INPUT);
	
	pinMode(SIG_R2, INPUT_PULLUP);
	pinMode(SIG_R3, OUTPUT);
	digitalWrite(SIG_R3, LOW);
	pinMode(SIG_L2, OUTPUT);
	digitalWrite(SIG_L2, LOW);
	pinMode(SIG_L3, INPUT_PULLUP);
	
	//SERIAL INIT
	Serial.begin(9600);
	softSerial_init();
  
	//NEOPIXEL INIT
	neopix_init();

	//DISPLAY INIT
	oled_init();	
}

void loop() 
{
	softSerial_routine();
	
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
		
		Serial.print((float)millis()/1000.f);	
		Serial.print("\t LVL:");
		Serial.print(level_getCurrLvl());
		
		Serial.print("\t");
		Serial.print(digitalRead(BTN_R)); Serial.print("\t");
		Serial.print(digitalRead(BTN_L)); Serial.print("\t");
		Serial.print(analogRead(BTN_A)%2); Serial.print("\t");
		Serial.print(analogRead(BTN_B)%2); Serial.print("\t");
		
		//cheat code
		if(digitalRead(BTN_U) == LOW)
		{
			if(level_getCurrLvl()<199)
				level_increment();
		}
		else if(digitalRead(BTN_D) == LOW)
		{
			if(level_getCurrLvl() > 1)
				level_decrement();
		}
		Serial.println();	
	}
  
  
	//--------------------------------------------
	//NEOPIXEL UPDATE
	static uint32_t ts_neopix = 0;
	if(millis() - ts_neopix > 1)
	{
		ts_neopix = millis();
		if(commLeftConnected == true)
		{
			if(mode_getWaitForDisconnect() == false)
			{
				incrementLoadingBar();
				neopix_connectLeft();
			}
			else
				neopix_success();		
				
		}
		else if(commRightConnected == true)
		{
			if(mode_getWaitForDisconnect() == false)
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
			mode_waitForDisconnect_deassert();	//clear loading bar variables
			mode_currentLoading_deassert();
			neopix_resetLoadingBar();
		}
	}

	
	//--------------------------------------------
	//DISPLAY UPDATE
	static uint32_t ts_oled = 0;
	if(millis() - ts_oled > 1000)
	{
		ts_oled = millis();
	}
}


