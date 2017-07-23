#ifndef OTAUpdate_h
#define OTAUpdate_h

#include <ArduinoOTA.h>

#define OTA_HOSTNAME "FroelingCom"

class OTAUpdate
{
  public:
    bool inProgress = false;
    void loop();
    void setup();
};

extern OTAUpdate otaUpdate;

#endif
