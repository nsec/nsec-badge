
//  -------------------------------------------------------------
//  -------------   Helper Function for OLED Display   ----------
//  -------------------------------------------------------------

void oled_bootScr(uint64_t id64, uint32_t id32)
{
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0,0);               // Start at top-left corner
  display.print(F(" * BOOTING *\nMAC: "));
  display.println(id64, HEX);
  display.print(F("crop 32: "));
  display.println(id32, HEX);
  display.display();
  
  delay(1500);  //wait before clearing the macAdr on screen
  display_home(); //show instruction on screen
}

void oled_test(void) 
{
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) 
  {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(500);
  display.clearDisplay();
  display.display();
}

void display_home()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F(" * NSEC25 Test FW *"));
  display.println(F("UP: RGB Test"));
  display.println(F("DW: OLED Test"));
  display.println(F("A : IR Transmit"));
  display.display();
}

void display_clear()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.display();
}