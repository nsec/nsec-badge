//  -------------------------------------------------------------
//  -------------   Helper Function  for Infrared   -------------   
//  -------------------------------------------------------------
uint16_t tx_forcedDelay = 1500;

void infrared_test()
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
  
    delay(1000);
    
    display_clear();
    IrReceiver.resume(); //Re-arm IR reception
  }
}

void infrared_listen()
{
  //  ================ IR Receiver - this is always listening  ================
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

        delay(1000);

        display_clear();
      }
    }
  }
}