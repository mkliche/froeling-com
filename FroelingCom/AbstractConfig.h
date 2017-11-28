#ifndef AbstractConfig_h
#define AbstractConfig_h

#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#include <ArduinoJson.h>

class AbstractConfig
{
  public:
    virtual void toJSON(JsonObject& json) = 0;
    virtual void fromJSON(JsonObject& json) = 0;
    virtual char* getKey() = 0;
    static constexpr const char* const LOG_TEMPLATE_NUMERIC = "Set %s to %d\r\n";
    static constexpr const char* const LOG_TEMPLATE_STRING = "Set %s to %s\r\n";

  protected:
    char* setStringProperty(char* propPtr, const char* value, const char* propName);
};

#endif
