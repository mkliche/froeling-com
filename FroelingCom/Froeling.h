#ifndef Froeling_h
#define Froeling_h

#include "AbstractConfig.h"
#include "froeling_com.h"

class Froeling : public AbstractConfig
{   
  public:
    void setup();
    void loop();

    void toJSON(JsonObject& json);
    void fromJSON(JsonObject& json);
    char* getKey();  
};

extern Froeling froeling;

#endif
