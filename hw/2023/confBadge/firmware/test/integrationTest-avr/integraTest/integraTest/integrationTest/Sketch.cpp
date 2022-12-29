#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//SOFTWARE SERIAL
SoftwareSerial rightSerial(6, 5); // RX, TX
SoftwareSerial leftSerial(2, 3); // RX, TX

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

//BUTTON
#define BTN_U	A1
#define BTN_D	A2
#define BTN_L	A3
#define BTN_R	A0

//NSEC COMM PORT
#define SIG_L1		2	
#define SIG_L2		3
#define SIG_L3		4
#define SIG_R1		5
#define SIG_R2		6
#define SIG_R3		7

//PROTOTYPE FUNCTION
void softSerialRoutine(void);
void neopix_idle(void);
void neopix_connectLeft(void);
void neopix_connectRight(void);
void incrementLoadingBar(void);
void neopix_success(void);
void display_idle(void);

//VARIABLE
const uint8_t MAX_MESSAGE_LENGTH = 8;
uint8_t currentLevel = 1;
bool	currentlyLoading = false;
bool	waitingForDisconnect = false;
uint8_t loadingBarPos = 0;
uint32_t idle_refreshInterval = 200;

void setup() 
{
	//GPIO INIT
	pinMode(13, OUTPUT);
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	pinMode(SIG_R2, INPUT_PULLUP);
	pinMode(SIG_R3, OUTPUT);
	digitalWrite(SIG_R3, LOW);
	pinMode(SIG_L2, OUTPUT);
	digitalWrite(SIG_L2, LOW);
	pinMode(SIG_L3, INPUT_PULLUP);
	
	//SERIAL INIT
	Serial.begin(9600);
	rightSerial.begin(4800);
	leftSerial.begin(4800);
  
	//NEOPIXEL INIT
	pixels.begin();

	//DISPLAY INIT
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
	{
	Serial.println(F("SSD1306 allocation failed"));
	}

	//OLED INIT
	display.setTextColor(SSD1306_WHITE);	
	display.clearDisplay();
	display.setTextSize(1); 
	/*
	display.print(F("John Smith"));
	display.startscrollleft(0x00, 0x0F);
	*/
	display.display();      // Show initial text
}

void loop() 
{
	softSerialRoutine();
	
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
		Serial.print(currentLevel);
		
		//cheat code
		if(digitalRead(BTN_U) == LOW)
		{
			//while(digitalRead(BTN_U) == LOW)
			{
				Serial.print("\t Level Up");
				if(currentLevel<199)
					currentLevel++;

				display.clearDisplay();
				display.setCursor(0,0);
				display.println();
				display.print(F("LVL="));
				display.print(currentLevel);
				display.display();
			}
		}
		else if(digitalRead(BTN_D) == LOW)
		{
			//while(digitalRead(BTN_D) == LOW)
			{
				Serial.print("\t Level Down");
				if(currentLevel > 1)
					currentLevel--;
				
				display.clearDisplay();
				display.setCursor(0,0);
				display.println();
				display.print(F("LVL="));
				display.print(currentLevel);
				display.display();
			}
		}
		
		Serial.println("");	
	}
  
  
	//--------------------------------------------
	//NEOPIXEL UPDATE
	static uint32_t ts_neopix = 0;
	if(millis() - ts_neopix > 1)
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
			/*
			if(waitingForDisconnect == true)	//if you just disconnect after pairing successfully. Return to showing the name
			{
				display.clearDisplay();
				display.setTextSize(2); // Draw 2X-scale text
				display.print(F("John Smith"));
				display.display();      // Show initial text
				display.startscrollleft(0x00, 0x0F);	
			}
			*/
			
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
		/*
		if(waitingForDisconnect == false)	//only show "pairing" if you just disconnected. After pairing is completed, "waitingForDisconnect" flag will be asserted. Another message will be shown
		{
			if		(commLeftConnected==true)
			{
				display.stopscroll();
				display.clearDisplay();
				display.setTextSize(2);
				display.setCursor(0,0);
				display.println(F("<<<PAIRING"));
				display.display();	
			}			
			else if (commRightConnected==true)
			{
				display.stopscroll();
				display.clearDisplay();
				display.setTextSize(2);
				display.setCursor(0,0);
				display.println(F("PAIRING>>>"));	
				display.display();	
			}
			else
			{
				
			}
		}
		*/
	}
}


//FUNCTION DECLARATION
void softSerialRoutine()
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
		display.clearDisplay();
		display.setCursor(0,0);
		display.print("broadcast:");
		display.println(sending);
		display.print("\nLeft RX:");
		display.println(receivedLeft);
		display.print("Right RX:");
		display.println(receivedRight);
		display.display();
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
		for(int i=0; i<currentLevel; i++)		//determine how many LED should be one
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
	if(currentlyLoading == false)
	{
		currentlyLoading = true;
		ts_loadingStartTime = millis();
		ts_loadingNextIncrement = ts_loadingStartTime + 1000;
		pixels.fill( pixels.Color(0,0,0), 8, 15);	//clear bottom row
		pixels.setPixelColor(15, pixels.Color(30,0,30) );
		pixels.show();
	}
	
	if(	millis() > ts_loadingNextIncrement && waitingForDisconnect == false)
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
			currentLevel++;
			if(currentLevel>=200)	
				currentLevel = 199;		//cap level at 199. No more additional LED animation after 199
			waitingForDisconnect = true;
			
			/*
			display.clearDisplay();
			display.setTextSize(2);
			display.setCursor(0,0);
			display.println(F("YAY LVL++"));
			display.print(F("LVL"));
			display.println(currentLevel);
			display.display();
			*/
		}
	}
}