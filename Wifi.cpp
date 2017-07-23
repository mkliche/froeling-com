#include "Wifi.h"

Wifi::Wifi() 
{
  
}

void Wifi::saveConfigCallback() 
{
  
}
void Wifi::setup() 
{
  pinMode(PIN_SETUP, INPUT);
  
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(Wifi::saveConfigCallback);

  //add all your parameters here
  
  if (digitalRead(PIN_SETUP) == LOW) 
  {
    WiFi.disconnect();
    if (!wifiManager.startConfigPortal("FroelingCom")) {
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
  } 
  else 
  {
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect("FroelingCom");
  }
}
