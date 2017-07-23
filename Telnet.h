#ifndef Telnet_h
#define Telnet_h

#include <ESP8266WiFi.h>
#include "SerialCommandDispatcher.h"
#include "Logging.h"

#define TELNET_MAX_SRV_CLIENTS 3
#define TELNET_READ_BUFFER 80

class Telnet
{
  public:
    Telnet();
    void loop();
    void setup();
    void send(const char* msg);
    void send(char c);
    void clearBuffer(uint8_t i);   // Clears the input buffer. 
  private:
    static WiFiServer server;
    String buffer[TELNET_MAX_SRV_CLIENTS];
    WiFiClient serverClients[TELNET_MAX_SRV_CLIENTS];
    void respond(uint8_t client, String response);
};

extern Telnet telnet;

#endif
