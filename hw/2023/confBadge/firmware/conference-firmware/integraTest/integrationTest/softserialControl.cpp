/*
 * softserialControl.cpp
 *
 * Created: 2022-12-20 2:31:48 PM
 *  Author: lambert
 */ 


#include <Arduino.h>
#include <SoftwareSerial.h>
#include "oledControl.h"


//VARIABLE
//SOFTWARE SERIAL
SoftwareSerial rightSerial(6, 5); // RX, TX
SoftwareSerial leftSerial(2, 3); // RX, TX
const uint8_t MAX_MESSAGE_LENGTH = 8;



//FUNCTION DECLARATION
void softSerial_init()
{
	rightSerial.begin(4800);
	leftSerial.begin(4800);
}

void softSerial_routine()
{
	static bool pingpongSelector = false;
	static uint32_t sending = 0;
	static char receivedRight[MAX_MESSAGE_LENGTH];
	static char receivedLeft[MAX_MESSAGE_LENGTH];
	
	static uint32_t ts_loop = 0;
	if(millis() - ts_loop > 1000)
	{
		ts_loop = millis();
		
		sending = millis();
		leftSerial.println(sending);
		rightSerial.println(sending);
	}
	
	static uint32_t ts_pingpong = 0;
	if(millis() - ts_pingpong > 200)
	{
		ts_pingpong = millis();
		pingpongSelector = !pingpongSelector;
	}
	
	static uint32_t ts_display = 0;
	if(millis() - ts_display > 100)
	{
		ts_display = millis();
		//oled_showSoftSerial();
	}
	
	
	static char msgRxRight[MAX_MESSAGE_LENGTH];
	static unsigned int msgRxRight_pos = 0;
	static char msgRxLeft[MAX_MESSAGE_LENGTH];
	static unsigned int msgRxLeft_pos = 0;
	if(pingpongSelector == false)
	{
		rightSerial.listen();
		while(rightSerial.available() > 0)
		{
			char inByte = rightSerial.read();
			if ( inByte != '\n' && (msgRxRight_pos < MAX_MESSAGE_LENGTH - 1) )
			{
				msgRxRight[msgRxRight_pos] = inByte; //Add the incoming byte to our message
				msgRxRight_pos++;
			}
			else
			{
				msgRxRight[msgRxRight_pos] = '\0';	//Add null character to string
				msgRxRight_pos = 0;				//Reset for the next message
				for(int i=0; i<MAX_MESSAGE_LENGTH; i++)
				{
					receivedRight[i] = msgRxRight[i];
				}
			}
		}
	}
	else
	{
		leftSerial.listen();
		while(leftSerial.available() > 0)
		{
			char inByte = leftSerial.read();
			if ( inByte != '\n' && (msgRxLeft_pos < MAX_MESSAGE_LENGTH - 1) )
			{
				msgRxLeft[msgRxLeft_pos] = inByte; //Add the incoming byte to our message
				msgRxLeft_pos++;
			}
			else
			{
				msgRxLeft[msgRxLeft_pos] = '\0';	//Add null character to string
				msgRxLeft_pos = 0;				//Reset for the next message
				for(int i=0; i<MAX_MESSAGE_LENGTH; i++)
				{
					receivedLeft[i] = msgRxLeft[i];
				}
			}
		}
	}
}
