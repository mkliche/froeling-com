#include <FS.h>
#include "HttpDispatcher.h"
#include "Logging.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "ConfigService.h"
#include "froeling_com.h"

ESP8266WebServer HttpDispatcher::server = ESP8266WebServer(80);
void HttpDispatcher::setup()
{  
  if (MDNS.begin("FroelingCom")) 
  {
    Log.info("MDNS responder started.\r\n");
  }

  if (SPIFFS.begin())
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) 
    {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Log.debug("FS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
  }  
  else
  {
    Log.error("Could not open SPIFFS for HttpDispatcher.\r\n");
  }

  server.onNotFound([this]() {
    if (!handleFileRead(server.uri()))
    {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, [](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"analog\":"+String(analogRead(A0));
    json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  server.on("/restart", HTTP_POST, [](){
    server.send(200);
    ESP.restart();
  });

  server.begin();
  Log.info("HTTP server started.\r\n");
}

void HttpDispatcher::loop()
{
  server.handleClient();
}

void HttpDispatcher::handleRoot()
{

  if (!SPIFFS.exists("/index.html")) {
    Log.error("No index.html found\r\n");
    return;
  }
  
  File f = SPIFFS.open("/index.html", "r");

  size_t size = f.size();
  // Allocate a buffer to store contents of the file.

  Log.info("Reading contents of index.html\r\n");

  String line = "";
  while(f.available()) {
    //Lets read line by line from the file
    line += f.readStringUntil('\n');
  }

  f.close();
  
  server.send(200, "text/plain", line.c_str());
}

void HttpDispatcher::handleError(char* message)
{
  Log.error("%s\r\n", message);
  String response = "{error: \"";
  response.concat(message);
  response.concat("\"}");
  server.send(400, "application/json", response);
}

void HttpDispatcher::registerJsonHandler(char* key, JsonHandlerFunction getHandler, JsonHandlerFunction postHandler)
{
  registerJsonGetHandler(key, getHandler);
  registerJsonPostHandler(key, postHandler);
}

void HttpDispatcher::registerJsonGetHandler(char* key, JsonHandlerFunction getHandler)
{
  String url = "/";
  url.concat(key);
    
  server.on(url.c_str(), HTTP_GET, [this, getHandler]() {
    sendJsonResponse(getHandler);
  });
}

void HttpDispatcher::registerJsonPostHandler(char* key, JsonHandlerFunction postHandler, bool saveConfig)
{
  String url = "/";
  url.concat(key);
  
  server.on(url.c_str(), HTTP_POST, [this, postHandler, saveConfig]() {

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(server.arg("plain"));
  
    if (!json.success()) 
    {
      handleError("Could not parse JSON request");  
      return;
    }
    postHandler(json);
    if (saveConfig)
    { 
      configService.save();
    }

    server.send(200);
  });
}

void HttpDispatcher::registerJsonPostHandler(char* key, JsonHandlerFunction postHandler)
{
  registerJsonPostHandler(key, postHandler, true);
}

void HttpDispatcher::sendJsonResponse(JsonHandlerFunction getHandler)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  getHandler(json);  
  String buffer;
  json.printTo(buffer);
  server.send(200, "application/json", buffer);
}

// FSBrowser
String HttpDispatcher::formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String HttpDispatcher::getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool HttpDispatcher::handleFileRead(String path){
  Log.debug("handleFileRead: %s\r\n", path.c_str());
  if(path.endsWith("/")) 
  {
    path += "index.html";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if(SPIFFS.exists(pathWithGz))
    {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    
    return true;
  }
  
  return false;
}

HttpDispatcher httpDispatcher = HttpDispatcher();
