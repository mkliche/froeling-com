#ifndef MQTT_h
#define MQTT_h

#define MQTT_MAX_PACKET_SIZE 2048

#include <PubSubClient.h>
#include "AbstractConfig.h"
#include "AbstractJsonDataBroker.h"
#include <ArduinoJson.h>

class MQTT : public AbstractConfig, public AbstractJsonDataBroker
{
  private:
  
    bool reconnect();

    long lastMsg = 0;
    long lastConnectAttempt = 0;
    
    String server = "";
    unsigned short port = 1883;
    String username = "";
    String password = "";
    String topic_prefix = "sensor/froeling";
    String client_name = "ESP8266";
    boolean enable = false;

    static PubSubClient client;
  
  public:
 
    void init();
    void setup();
    void loop();

    void toJSON(JsonObject& json);
    void fromJSON(JsonObject& json);
    char* getKey();

    unsigned long getInterval();

    static constexpr const char* const COLUMN_SERVER = "server";
    static constexpr const char* const COLUMN_PORT = "port";
    static constexpr const char* const COLUMN_USERNAME = "username";
    static constexpr const char* const COLUMN_PASSWORD = "password";
    static constexpr const char* const COLUMN_TOPIC_PREFIX = "topic_prefix";
    static constexpr const char* const COLUMN_CLIENT_NAME = "client_name";
    static constexpr const char* const COLUMN_ENABLE = "enable";
};

extern MQTT mqtt;

#endif
