// #include <FS.h> //this needs to be first, or it all crashes and burns...

#include "Logging.h"
// #include "ConfigService.h"
#include "Wifi.h"
#include "Telnet.h"
// #include "SerialCommandDispatcher.h"
#include "OTAUpdate.h"
#include "HttpDispatcher.h"
#include "FTP.h"
#include "Froeling.h"
#include "MQTT.h"
#include "ThingSpeakBroker.h"
#include "ConfigService.h"

#define GREEN_LED 13
#define RED_LED 14

void setup() {

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Log.init(LOGLEVEL, 9600);
  ftp.init();
  mqtt.init();
  tsBroker.init();
  
  Wifi::setup();
  otaUpdate.setup();
  // dispatcher.setup();  
  
  httpDispatcher.setup();
  telnet.setup();
  
  configService.setup();
  ftp.setup();  
  froeling.setup();  
  tsBroker.setup();
  mqtt.setup();  
}

void loop() {
  // put your main code here, to run repeatedly:
  otaUpdate.loop();
  if (!otaUpdate.inProgress) {
    froeling.loop();
    // dispatcher.loop();
    telnet.loop();
    httpDispatcher.loop();
    ftp.loop();
    mqtt.loop();
    tsBroker.loop();
  }
}
