#include "MQTT.h"
#include "Logging.h"
#include "ConfigService.h"
#include "HttpDispatcher.h"

PubSubClient MQTT::client = PubSubClient(AbstractJsonDataBroker::espClient);

void MQTT::init()
{
  configService.registerConfig(this);
}

void MQTT::setup()
{  
  httpDispatcher.registerJsonHandler(getKey(), std::bind(&MQTT::toJSON, this, std::placeholders::_1), std::bind(&MQTT::fromJSON, this, std::placeholders::_1));
  client.setServer(server.c_str(), port);
}

unsigned long MQTT::getInterval()
{
  return 60;
}

void MQTT::loop()
{
  if (!enable)
  {
    return;
  }
  
  if (server == "") 
  {    
    return;
  }
  
  if (!client.connected()) 
  {
    if (!reconnect())
    {
      return;
    }
  }
  
  client.loop();

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

      String json_buffer;
      json.printTo(json_buffer);
      String topic = topic_prefix + "/all";
      client.publish(topic.c_str(), json_buffer.c_str()); 
     
      providers[i]->lastMessage = now;      
    }
  }
}

bool MQTT::reconnect()
{   
  // Loop until we're reconnected
  if (!client.connected() && millis() - lastConnectAttempt > 5000)
  {
    
    Log.info("Attempting MQTT connection... ");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect(client_name.c_str(), username.c_str(), password.c_str())) 
    {
      Log.info("connected\r\n");
      lastConnectAttempt = millis();

      return true;
    } 
    else 
    {
      Log.info("failed (rc=%d) - try again in 5 seconds\r\n", client.state());
      lastConnectAttempt = millis();

      return false;
    }
  }
}

char* MQTT::getKey()
{
  return "mqtt";
}

void MQTT::toJSON(JsonObject& json)
{
  json[MQTT::COLUMN_SERVER] = server;
  json[MQTT::COLUMN_PORT] = port;
  json[MQTT::COLUMN_USERNAME] = username;
  json[MQTT::COLUMN_PASSWORD] = password;   
  json[MQTT::COLUMN_TOPIC_PREFIX] = topic_prefix;   
  json[MQTT::COLUMN_CLIENT_NAME] = client_name;   
  json[MQTT::COLUMN_ENABLE] = enable; 
}

void MQTT::fromJSON(JsonObject& json)
{
  if (json.containsKey(MQTT::COLUMN_SERVER))
  {
    server = json.get<String>(MQTT::COLUMN_SERVER);
  }
  if (json.containsKey(MQTT::COLUMN_PORT))
  {
    port = json.get<unsigned short>(MQTT::COLUMN_PORT);
  }
  if (json.containsKey(MQTT::COLUMN_USERNAME))
  {
    username = json.get<String>(MQTT::COLUMN_USERNAME);
  }
  if (json.containsKey(MQTT::COLUMN_PASSWORD))
  {
    password = json.get<String>(MQTT::COLUMN_PASSWORD);
  }
  if (json.containsKey(MQTT::COLUMN_TOPIC_PREFIX))
  {
    topic_prefix = json.get<String>(MQTT::COLUMN_TOPIC_PREFIX);
  }      
  if (json.containsKey(MQTT::COLUMN_CLIENT_NAME))
  {
    client_name = json.get<String>(MQTT::COLUMN_CLIENT_NAME);
  }
  if (json.containsKey(MQTT::COLUMN_ENABLE))
  {
    enable = json.get<boolean>(MQTT::COLUMN_ENABLE);
  }  

  client.setServer(server.c_str(), port);
}

MQTT mqtt = MQTT();
