#include "FTP.h"
#include "Logging.h"
#include "ConfigService.h"
#include "HttpDispatcher.h"
#include <functional>

FtpServer FTP::srv = FtpServer();

void FTP::init()
{
  configService.registerConfig(this);
}

void FTP::setup()
{
  Log.debug("FTP Setup\r\n");
  if (!configService.load())
  {
    Log.error("Could not load FTP server config\r\n");

    return;
  }
  if (SPIFFS.begin()) {
    srv.begin(username, password); 
  }

  httpDispatcher.registerJsonHandler(getKey(), std::bind(&FTP::toJSON, this, std::placeholders::_1), std::bind(&FTP::fromJSON, this, std::placeholders::_1));
}

void FTP::loop()
{
  srv.handleFTP();  
}


char* FTP::getKey()
{
  return "ftp";
}

void FTP::toJSON(JsonObject& json)
{
  json[FTP::COLUMN_USERNAME] = username;
  json[FTP::COLUMN_PASSWORD] = password; 
}

void FTP::fromJSON(JsonObject& json)
{
  if (json.containsKey(FTP::COLUMN_USERNAME))
  {
    username = json.get<String>(FTP::COLUMN_USERNAME);
  }  
  if (json.containsKey(FTP::COLUMN_PASSWORD))
  {
    password = json.get<String>(FTP::COLUMN_PASSWORD);
  }
}

FTP ftp = FTP();
