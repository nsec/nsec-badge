//  -------------------------------------------------------------
//  -------------   Helper Function  for Neopixel   -------------   
//  -------------------------------------------------------------
void neopix_test()
{
  static uint8_t state = 0;
  switch(state)
  {
    case 0:
      colorWipe(strip.Color(  0,   0, 128), 50); // Blue
      state=1;
      break;
    case 1:
      colorWipe(strip.Color(  0, 128,   0), 50); // Green
      state=2;
      break;
    case 2:
      colorWipe(strip.Color(128,   0,   0), 50); // Red
      state=0;
      break;
    default:
      colorWipe(strip.Color(  0,   0, 128), 50); // Blue
      state=1;
      break;
  }
  strip.show();
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}