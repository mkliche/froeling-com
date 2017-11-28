#ifndef AbstractJsonDataBroker_h
#define AbstractJsonDataBroker_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class JsonDataProvider;

class AbstractJsonDataBroker
{
  protected:

    JsonDataProvider** providers;
    uint8_t providerCount = 0;

    static WiFiClient espClient;
  
  public:

    virtual void loop() = 0;
    virtual void setup() {};
    virtual unsigned long getInterval() = 0;

    void registerJsonDataProvider(std::function<void(JsonObject&)> dataProvider);
};

class JsonDataProvider
{
  public:
  
    unsigned long interval = 60;
    unsigned long lastMessage = 0;
    std::function<void(JsonObject&)> toJSON;
};

#endif
