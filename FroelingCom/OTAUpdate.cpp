#include <FS.h>
#include "OTAUpdate.h"
#include "Logging.h"

void OTAUpdate::setup() 
{  
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    SPIFFS.end();
    otaUpdate.inProgress = true;
    Log.info("Starting OTA update\r\n");
  });
  
  ArduinoOTA.onEnd([]() {
    otaUpdate.inProgress = false;
    Log.info("Finished OTA update\r\n");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.info("OTA update progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    otaUpdate.inProgress = false;
    Log.error("OTA update error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Log.error("Auth failed\r\n");
    else if (error == OTA_BEGIN_ERROR) Log.error("Begin failed\r\n");
    else if (error == OTA_CONNECT_ERROR) Log.error("Connect failed\r\n");
    else if (error == OTA_RECEIVE_ERROR) Log.error("Receive failed\r\n");
    else if (error == OTA_END_ERROR) Log.error("End failed\r\n");
  });
  
  ArduinoOTA.begin();

  Log.info("OTA update is active.\r\n");
}

void OTAUpdate::loop() 
{
  ArduinoOTA.handle();
}

OTAUpdate otaUpdate = OTAUpdate();
