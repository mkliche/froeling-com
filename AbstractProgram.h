#ifndef AbstractProgram_h
#define AbstractProgram_h

#include "AbstractConfig.h"

class AbstractProgram : virtual public AbstractConfig
{
  public:
    virtual void loop() = 0;
    virtual void setup() {};
    virtual void activate() {};
    virtual void deactivate() {};
};

#endif
