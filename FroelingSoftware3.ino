// #include <FS.h> //this needs to be first, or it all crashes and burns...

#include "Logging.h"
// #include "ConfigService.h"
#include "Wifi.h"
#include "Telnet.h"
// #include "SerialCommandDispatcher.h"
#include "OTAUpdate.h"
#include "HttpDispatcher.h"
#include "froeling_com.h"

#define GREEN_LED 13
#define RED_LED 14

#define LOGLEVEL LOG_LEVEL_DEBUG

void setup() {

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Log.init(LOGLEVEL, 9600);
  
  Wifi::setup();
  otaUpdate.setup();
  // dispatcher.setup();
  httpDispatcher.setup();
  telnet.setup();
  // configService.setup();

  FroeCom_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  otaUpdate.loop();
  if (!otaUpdate.inProgress) {
    FroeComSTM();
    // dispatcher.loop();
    telnet.loop();
    httpDispatcher.loop();
  }
}
