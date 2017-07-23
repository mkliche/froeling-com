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

  server.on("/data", HTTP_GET, HttpDispatcher::handleData);

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

void HttpDispatcher::handleData()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["tCurrentMainState"] = tFroeComStmData.tCurrentMainState;
  json["tCurrentSubState"] = tFroeComStmData.tCurrentSubState;
  json["cReceiveBuffer"] = tFroeComStmData.cReceiveBuffer;
  json["iInBuffPointer"] = tFroeComStmData.iInBuffPointer;
  //json["cRbSendFlag"] = tFroeComStmData.cRbSendFlag;
  json["iMsgLength"] = tFroeComStmData.iMsgLength;
  json["glsUserID"] = FROELING_USER_ID;
  json["iLastFroelingTimestamp"] = tFroeComStmData.iLastFroelingTimestamp;
  json["ulLastSystemTimestamp"] = tFroeComStmData.ulLastSystemTimestamp;
  json["status"] = froeling_proc_value_array[0];
  json["zustand"] = froeling_proc_value_array[1];
  json["rost"] = froeling_proc_value_array[2];
  json["kesseltemp"] = froeling_proc_value_array[3];
  json["abgastemp"] = froeling_proc_value_array[4];
  json["abgas_sw"] = froeling_proc_value_array[5];
  json["kess_stell_gr"] = froeling_proc_value_array[6];
  json["saugzug"] = froeling_proc_value_array[7];
  json["prim_luft"] = froeling_proc_value_array[8];
  json["prim_kl_pos"] = froeling_proc_value_array[9];
  json["rest_o2"] = froeling_proc_value_array[10];
  json["o2_regler"] = froeling_proc_value_array[11];
  json["sek_luft"] = froeling_proc_value_array[12];
  json["sek_kl_pos"] = froeling_proc_value_array[13];
  json["puffer_oben"] = froeling_proc_value_array[14];
  json["puffer_unten"] = froeling_proc_value_array[15];
  json["puffer_pu"] = froeling_proc_value_array[16];
  json["boilertemp"] = froeling_proc_value_array[17];
  json["oelkessel"] = froeling_proc_value_array[18];
  json["aussentemp"] = froeling_proc_value_array[19];
  json["vorlauftemp_1sw"] = froeling_proc_value_array[20];
  json["vorlauftemp_1"] = froeling_proc_value_array[21];
  json["kty6_h2"] = froeling_proc_value_array[22];
  json["kty7_h2"] = froeling_proc_value_array[23];
  json["laufzeit"] = froeling_proc_value_array[24];
  json["f_halt"] = froeling_proc_value_array[25];
  json["boardtemp"] = froeling_proc_value_array[26];  

  char out[1024];
  json.printTo(out, sizeof(out));
  
  server.send(200, "application/json", out);
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
  String url = "/";
  url.concat(key);
  server.on(url.c_str(), HTTP_GET, [this, getHandler]() {
    sendJsonResponse(getHandler);
  });
  server.on(url.c_str(), HTTP_POST, [this, getHandler, postHandler]() {
  
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(server.arg("plain"));
  
    if (!json.success()) 
    {
      handleError("Could not parse JSON request");  
      return;
    }
    postHandler(json);
    configService.save();
    sendJsonResponse(getHandler);
  });
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
    path += "index.htm";
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
