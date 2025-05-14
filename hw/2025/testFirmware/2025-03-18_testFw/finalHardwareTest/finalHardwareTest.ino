//DEFINE OLED SCREEN
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//DEFINE NEOPIXEL
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

//DEFINE BUTTONS
#define BTN_UP 0
#define BTN_DN 1
#define BTN_A 9

void setup() 
{
  //UART Setup
  Serial.begin(115200);

  //Infrared Setup
  uint64_t myId64 = ESP.getEfuseMac(); //Full mac-address
  myId = ESP.getEfuseMac()>>16; //take the first 32bit of the macaddress (that's the unique-ID)
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);//--------- RECEIVER ----------
  IrSender.begin(); //--------- TRANSMITTER ---------- (Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin)
  disableLEDFeedback(); // Disable feedback LED at default feedback LED pin

  //OLED Setup
  Wire.setPins(4,5);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // Don't proceed, loop forever
  }
  delay(50); //wait for oled hw init
  oled_bootScr(myId64, myId);

  //RGB Setup
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  neopix_test();

  //BTN Setup
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DN, INPUT);
  pinMode(BTN_A, INPUT);
}

void loop() 
{
  //  IR RECEIVER
  infrared_listen();  // this is always listening

  //  BUTTONS
  if(digitalRead(BTN_UP)==LOW)        //  TEST NEOPIXEL - Press Up
  {
    neopix_test();
    display_home();
  }
  else if(digitalRead(BTN_DN)==LOW)   //  TEST OLED - Press Down
  {
    oled_test();
    display_home();
  }
  else if(digitalRead(BTN_A) == LOW)  //  TEST IR Transmit - Press A
  {
    infrared_test();
    display_home();
  }
}
