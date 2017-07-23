#ifndef ConfigService_h
#define ConfigService_h

#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#include <ArduinoJson.h>
#include "AbstractConfig.h"

enum class ConfigProperty {
  ntpHost,
  ledProgram
};

class ConfigService
{
  private:
    AbstractConfig** configs;
    uint8_t configCount = 0;
    bool isLoaded = false;
  
  public:
    void setup();
    bool load(bool forceReload = false);
    void save();
    void registerConfig(AbstractConfig* config);   
    
};

extern ConfigService configService;

#endif
