#include "SystemConfig.h"

char* SystemConfig::getKey()
{
  return "system";
}
void SystemConfig::toJSON(JsonObject& json)
{
  json[SystemConfig::COLUMN_MIN_BRIGHTNESS] = minBrightness;
  json[SystemConfig::COLUMN_MAX_BRIGHTNESS] = maxBrightness;  
  json[SystemConfig::COLUMN_OFFSET_BRIGHTNESS] = offsetBrightness;
}
void SystemConfig::fromJSON(JsonObject& json)
{
  if (json.containsKey(SystemConfig::COLUMN_MIN_BRIGHTNESS))
  {
    minBrightness = json.get<unsigned short>(SystemConfig::COLUMN_MIN_BRIGHTNESS);
  }  
  if (json.containsKey(SystemConfig::COLUMN_MAX_BRIGHTNESS))
  {
    maxBrightness = json.get<unsigned short>(SystemConfig::COLUMN_MAX_BRIGHTNESS);
  }
  if (json.containsKey(SystemConfig::COLUMN_OFFSET_BRIGHTNESS))
  {
    offsetBrightness = json.get<signed short>(SystemConfig::COLUMN_OFFSET_BRIGHTNESS);
  }
}

SystemConfig systemConfig = SystemConfig();
