#ifndef HttpDispatcher_h
#define HttpDispatcher_h

#include <FS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <functional>

class HttpDispatcher
{
  public:

    typedef std::function<void(JsonObject&)> JsonHandlerFunction;
  
    void setup();
    void loop();
    static void handleRoot();
    static void handleData();
    static void handleError(char* message);
    static bool handleFileRead(String path);
    void registerJsonHandler(char* key, JsonHandlerFunction getHandler, JsonHandlerFunction postHandler);
    void registerJsonGetHandler(char* key, JsonHandlerFunction getHandler);
    void registerJsonPostHandler(char* key, JsonHandlerFunction postHandler);
    void registerJsonPostHandler(char* key, JsonHandlerFunction postHandler, bool saveConfig);
    static ESP8266WebServer server;    
  
  private:
    static void sendJsonResponse(JsonHandlerFunction getHandler);

    // FSBrowser
    static String formatBytes(size_t bytes);
    static String getContentType(String filename);
    
    
    
};

extern HttpDispatcher httpDispatcher;

#endif
