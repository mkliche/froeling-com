#include <FS.h>
#include "ConfigService.h"
#include "Logging.h"

#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#include <ArduinoJson.h>

void ConfigService::setup()
{  
  if (load()) {
    Log.debug("Loaded config successfully\r\n");
  } else {
    Log.debug("Could not load config, rewriting default config to flash\r\n");
    save();
  }
}

bool ConfigService::load(bool forceReload)
{
  if (isLoaded && !forceReload)
  {
    return true;
  }

  if (!SPIFFS.begin()) {
    Log.error("File system could not be initialized\r\n");
    return false;
  }

  if (!SPIFFS.exists("/config.json")) {
    Log.error("No config.json found\r\n");
    return false;
  }
  
  //file exists, reading and loading
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Log.error("Could not open config.json\r\n");
    return false;
  }
    
  size_t size = configFile.size();
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  Log.info("Reading contents of config.json\r\n");
  configFile.readBytes(buf.get(), size);
  Log.verbose("Contents of config.json: %s\r\n", buf.get());
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Log.error("Failed parsing contents of config.json\r\n");
    return false;
  }

  for (uint8_t i = 0; i < configCount; i++)
  {
    if (json.containsKey(configs[i]->getKey()))
    {
      configs[i]->fromJSON(json.get<JsonVariant>(configs[i]->getKey()));
    }
  }

  configFile.close();
  isLoaded = true;
  
  return true;
}

/*
void ConfigService::fromJson(JsonObject& json)
{
  for (uint8_t i = 0; i < sizeof(configPropertyNames)/sizeof(configPropertyNames[0]); i++) {

    Log.verbose("Looking for property %s in JSON...", configPropertyNames[i]);
    
    if (json.containsKey(configPropertyNames[i])) {
      Log.verbose(" found\r\n");
      ConfigProperty prop = static_cast<ConfigProperty>(i);
      switch (prop) {
        case ConfigProperty::ntpHost:
          setNtpHost(json.get<const char*>(configPropertyNames[i]));
          break;
      }
    } else {
      Log.verbose(" not found\r\n");
    }
  }
}
void ConfigService::toJson(JsonObject& json)
{
  json["ntp_host"] = ntpHost;
}
*/

void ConfigService::save()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  for (uint8_t i = 0; i < configCount; i++)
  {
    JsonObject& configJson = json.createNestedObject(configs[i]->getKey());
    configs[i]->toJSON(configJson);    
  }

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Log.error("Could not open config.json for writing\r\n");
    
    return;
  }

  Log.debug("Writing config to config.json...");
  json.printTo(configFile);
  Log.debug(" done\r\n");
  configFile.close();
}

void ConfigService::registerConfig(AbstractConfig* config)
{
  configs = (AbstractConfig**)realloc(configs, ++configCount * sizeof(AbstractConfig*));
  if (NULL == configs)
  {
    Log.error("Could not allocate memory for config %s\r\n", config->getKey());

    return;
  }

  configs[configCount - 1] = config;
}

ConfigService configService = ConfigService();
