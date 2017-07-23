#ifndef SystemConfig_h
#define SystemConfig_h

#include <Arduino.h>
#include "AbstractConfig.h"
#include "ConfigService.h"

class SystemConfig : public AbstractConfig
{
  public:
    uint8_t minBrightness = 1;
    uint8_t maxBrightness = 255;
    int8_t offsetBrightness = 0;
    void toJSON(JsonObject& json);
    void fromJSON(JsonObject& json);
    char* getKey();

    static constexpr const char* const COLUMN_MAX_BRIGHTNESS = "max_brightness";
    static constexpr const char* const COLUMN_MIN_BRIGHTNESS = "min_brightness";
    static constexpr const char* const COLUMN_OFFSET_BRIGHTNESS = "offset_brightness";

};

extern SystemConfig systemConfig;

#endif
