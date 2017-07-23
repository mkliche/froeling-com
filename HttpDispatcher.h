#ifndef HttpDispatcher_h
#define HttpDispatcher_h

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
    void registerJsonHandler(char* key, JsonHandlerFunction getHandler, JsonHandlerFunction postHandler);
    static ESP8266WebServer server;
  
  private:
    void sendJsonResponse(JsonHandlerFunction getHandler);

    // FSBrowser
    String formatBytes(size_t bytes);
    String getContentType(String filename);
    bool handleFileRead(String path);
    
    
};

extern HttpDispatcher httpDispatcher;

#endif
