//INCLUDE OLED SCREEN
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//INCLUDE NEOPIXEL
#include <Adafruit_NeoPixel.h>
#define LED_PIN    8
#define LED_COUNT 18
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//DEFINE IR TRANSCEIVER
#if !defined(ARDUINO_ESP32C3_DEV) // This is due to a bug in RISC-V compiler, which requires unused function sections :-(.
  #define DISABLE_CODE_FOR_RECEIVER // Disables static receiver code like receive timer ISR handler and static IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not required.
#endif
#define IR_RECEIVE_PIN           3
#define IR_SEND_PIN             10
#include <IRremote.hpp> // include the library
uint32_t myId = 0;
uint16_t tx_forcedDelay = 1500;

//DEFINE BUTTONS
#define BTN_UP 0
#define BTN_DN 1
#define BTN_A 9

//Prototype function
void neopix_rainbow(int wait);
void neopix_clear(void);
void neopix_green(void);
void neopix_blue(void);
void testdrawcircle(void);
void display_clear(void);

void setup() 
{
  Serial.begin(115200);

  //OLED Setup
  Wire.setPins(4,5);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("NSEC 2025 Test FW."));

  //Neopixel Setup
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  //Infrared Setup
  uint64_t myId64 = ESP.getEfuseMac(); //Full mac-address
  myId = ESP.getEfuseMac()>>16; //take the first 32bit of the macaddress (that's the unique-ID)
  display.printf("MAC: %llx\n", myId64);
  display.printf("cropped MAC (32bit): %x\n", myId);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);//--------- RECEIVER ----------
  IrSender.begin(); //--------- TRANSMITTER ---------- // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
  disableLEDFeedback(); // Disable feedback LED at default feedback LED pin

  //Buttons Setup
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DN, INPUT);
  pinMode(BTN_A, INPUT);
}

void loop() 
{  
  //  -------------   IR Receiver - this is always listening
  if (IrReceiver.decode())  
  {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) 
    {
      uint32_t receivedTemp = IrReceiver.decodedIRData.decodedRawData;
      if(receivedTemp == myId)
        delay(0);//ignore reflection from my own transmission
      else
      {
        IrReceiver.resume(); // Early enable receiving of the next IR frame

        display_clear();
        display.print(F(" >>> RX: "));
        display.println(IrReceiver.decodedIRData.decodedRawData, HEX);  //print packet to OLED
        display.display();
        
        neopix_blue(); //turn neopixel green when receiving a packet
        delay(1000);

        display_clear();
        neopix_clear();
      }
    }
  }

  //  -------------   TEST NEOPIXEL - Press Up
  if(digitalRead(BTN_UP)==LOW)
  {
    display_clear();
    display.println(F("Test Neopixel"));
    display.display();
    neopix_rainbow(5); //Flowing rainbow cycle along the whole strip
    display_clear();
    neopix_clear();
  }

  //  -------------   TEST OLED - Press Down
  if(digitalRead(BTN_DN)==LOW)
  {
    display_clear();
    display.println(F("Test OLED"));
    display.display();
    delay(750);
    testdrawcircle();
    display_clear();
  }

  //  -------------   TEST IR transmission - Press A
  if(digitalRead(BTN_A) == LOW) 
  {
    static uint32_t time_lastSent = 0;
    if(millis() - time_lastSent > tx_forcedDelay)   //force a random delay between transmission, force socialization between participants
    {
      time_lastSent = millis();
      display_clear();
      display.print(F(" <<< TX: "));
      display.print(myId, HEX);
      display.display();
      
      IrSender.sendNECRaw(myId, 0);
      IrReceiver.resume();
      
      neopix_green();
      delay(1000);

      display_clear();
      neopix_clear();
      IrReceiver.resume(); //Re-arm IR reception
    }
  }

  //update OLED screen
  display.display();
}


//  ----------------------------------------
//  -------------   FUNCITON   -------------   
void neopix_rainbow(int wait) //Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
{
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 512) 
  {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void neopix_clear()
{
  strip.fill(strip.Color(0,0,0), 0, 18);
  strip.show();
}

void neopix_green()
{
  strip.clear();
  for(int i=0; i<12; i++)
  {
    strip.setPixelColor(i, strip.Color(0,128,0));
    strip.show();
    delay(25);
  }
}

void neopix_blue()
{
  strip.clear();
  for(int i=12; i<18; i++)
  {
    strip.setPixelColor(i, strip.Color(0,0,128));
    strip.show();
    delay(25);
  }
}

void testdrawcircle(void) 
{
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) 
  {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(1000);
}

void display_clear()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.display();
}