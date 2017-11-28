#ifndef Wifi_h
#define Wifi_h

#define PIN_SETUP 12

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>   

class Wifi
{
  public:
    Wifi();
    static void setup();
    static void saveConfigCallback();
};

#endif
