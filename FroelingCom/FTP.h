#ifndef FTP_h
#define FTP_h

#include <String.h>
#include <ESP8266FtpServer.h>
#include "AbstractConfig.h"

class FTP : public AbstractConfig
{
  private:
    String username = "ftp";
    String password = "ftp";
    static FtpServer srv;
    
  
  public:
    void init();
    void setup();
    void loop();

    void toJSON(JsonObject& json);
    void fromJSON(JsonObject& json);
    char* getKey();

    static constexpr const char* const COLUMN_USERNAME = "username";
    static constexpr const char* const COLUMN_PASSWORD = "password";
    
};

extern FTP ftp;

#endif
