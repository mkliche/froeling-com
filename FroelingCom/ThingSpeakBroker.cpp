#include "ThingSpeakBroker.h"
#include "Logging.h"
#include "ConfigService.h"
#include "HttpDispatcher.h"

#include <ThingSpeak.h>

void ThingSpeakBroker::init()
{
  configService.registerConfig(this);
}

void ThingSpeakBroker::setup()
{  
  httpDispatcher.registerJsonHandler(getKey(), std::bind(&ThingSpeakBroker::toJSON, this, std::placeholders::_1), std::bind(&ThingSpeakBroker::fromJSON, this, std::placeholders::_1));
  ThingSpeak.begin(AbstractJsonDataBroker::espClient);

  Log.debug("Interval: %d\r\n", interval);
}

unsigned long ThingSpeakBroker::getInterval()
{ 
  return interval;
}

void ThingSpeakBroker::loop()
{ 
  if (!enable || !channel || write_key == "") 
  {    
    return;
  }
  
  long now = millis();
  const char* key;
  const char* value;

  for (uint8_t i = 0; i < providerCount; i++)
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    
    if (now - providers[i]->lastMessage > providers[i]->interval)
    {
      providers[i]->toJSON(json);

      if (field_1 != "" && json.containsKey(field_1))
      {
        ThingSpeak.setField(1, json.get<String>(field_1));
      }

      if (field_2 != "" && json.containsKey(field_2))
      {
        ThingSpeak.setField(2, json.get<String>(field_2));
      }
      
      if (field_3 != "" && json.containsKey(field_3))
      {
        ThingSpeak.setField(3, json.get<String>(field_3));
      }
      
      if (field_4 != "" && json.containsKey(field_4))
      {
        ThingSpeak.setField(4, json.get<String>(field_4));
      }
      
      if (field_5 != "" && json.containsKey(field_5))
      {
        ThingSpeak.setField(5, json.get<String>(field_5));
      }
      
      if (field_6 != "" && json.containsKey(field_6))
      {
        ThingSpeak.setField(6, json.get<String>(field_6));
      }
      
      if (field_7 != "" && json.containsKey(field_7))
      {
        ThingSpeak.setField(7, json.get<String>(field_7));
      }
      
      if (field_8 != "" && json.containsKey(field_8))
      {
        ThingSpeak.setField(8, json.get<String>(field_8));
      }      

      Log.info("Sending ThingSpeak data\r\n");
      ThingSpeak.writeFields(channel, write_key.c_str());
     
      providers[i]->lastMessage = now;      
    }
  }
}

char* ThingSpeakBroker::getKey()
{
  return "thingspeak";
}

void ThingSpeakBroker::toJSON(JsonObject& json)
{
  json[ThingSpeakBroker::COLUMN_CHANNEL] = channel;
  json[ThingSpeakBroker::COLUMN_WRITE_KEY] = write_key;
  json[ThingSpeakBroker::COLUMN_INTERVAL] = interval;
  json[ThingSpeakBroker::COLUMN_FIELD_1] = field_1;
  json[ThingSpeakBroker::COLUMN_FIELD_2] = field_2;
  json[ThingSpeakBroker::COLUMN_FIELD_3] = field_3;
  json[ThingSpeakBroker::COLUMN_FIELD_4] = field_4;
  json[ThingSpeakBroker::COLUMN_FIELD_5] = field_5;
  json[ThingSpeakBroker::COLUMN_FIELD_6] = field_6;
  json[ThingSpeakBroker::COLUMN_FIELD_7] = field_7;
  json[ThingSpeakBroker::COLUMN_FIELD_8] = field_8;
  json[ThingSpeakBroker::COLUMN_ENABLE] = enable; 
}

void ThingSpeakBroker::fromJSON(JsonObject& json)
{
  if (json.containsKey(ThingSpeakBroker::COLUMN_CHANNEL))
  {
    channel = json.get<unsigned long>(ThingSpeakBroker::COLUMN_CHANNEL);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_WRITE_KEY))
  {
    write_key = json.get<String>(ThingSpeakBroker::COLUMN_WRITE_KEY);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_INTERVAL))
  {
    interval = json.get<unsigned long>(ThingSpeakBroker::COLUMN_INTERVAL);
  }  
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_1))
  {
    field_1 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_1);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_2))
  {
    field_2 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_2);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_3))
  {
    field_3 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_3);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_4))
  {
    field_4 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_4);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_5))
  {
    field_5 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_5);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_6))
  {
    field_6 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_6);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_7))
  {
    field_7 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_7);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_FIELD_8))
  {
    field_8 = json.get<String>(ThingSpeakBroker::COLUMN_FIELD_8);
  }
  if (json.containsKey(ThingSpeakBroker::COLUMN_ENABLE))
  {
    enable = json.get<boolean>(ThingSpeakBroker::COLUMN_ENABLE);
  } 
}

ThingSpeakBroker tsBroker = ThingSpeakBroker();
