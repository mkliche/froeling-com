#ifndef ThingSpeakBroker_h
#define ThingSpeakBroker_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "AbstractConfig.h"
#include "AbstractJsonDataBroker.h"
#include <ArduinoJson.h>

class ThingSpeakBroker : public AbstractConfig, public AbstractJsonDataBroker
{
  private:

    unsigned long channel = 0;
    String write_key = "";
    unsigned long interval = 60;
    String field_1 = "";
    String field_2 = "";
    String field_3 = "";
    String field_4 = "";
    String field_5 = "";
    String field_6 = "";
    String field_7 = "";
    String field_8 = "";
    boolean enable = false;

    static WiFiClient espClient;
    static PubSubClient client;
  
  public:
 
    void init();
    void setup();
    void loop();

    void toJSON(JsonObject& json);
    void fromJSON(JsonObject& json);
    char* getKey();

    unsigned long getInterval();

    static constexpr const char* const COLUMN_CHANNEL = "channel";
    static constexpr const char* const COLUMN_WRITE_KEY = "write_key";
    static constexpr const char* const COLUMN_INTERVAL = "interval";
    static constexpr const char* const COLUMN_FIELD_1 = "field_1";
    static constexpr const char* const COLUMN_FIELD_2 = "field_2";
    static constexpr const char* const COLUMN_FIELD_3 = "field_3";
    static constexpr const char* const COLUMN_FIELD_4 = "field_4";
    static constexpr const char* const COLUMN_FIELD_5 = "field_5";
    static constexpr const char* const COLUMN_FIELD_6 = "field_6";
    static constexpr const char* const COLUMN_FIELD_7 = "field_7";
    static constexpr const char* const COLUMN_FIELD_8 = "field_8";
    static constexpr const char* const COLUMN_ENABLE = "enable";
};

extern ThingSpeakBroker tsBroker;

#endif
