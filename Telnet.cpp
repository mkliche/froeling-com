#include "Telnet.h"

Telnet::Telnet()
{
}

WiFiServer Telnet::server(23);

void Telnet::send(const char* msg)
{
  unsigned int msgLength = strlen(msg);
  for(uint8_t i = 0; i < TELNET_MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      serverClients[i].write(msg, msgLength);
    }
  }  
}

void Telnet::send(char c)
{
  for(uint8_t i = 0; i < TELNET_MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      serverClients[i].write((uint8_t)c);
    }
  }
}

void Telnet::loop() 
{
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient())
  {
    for (i = 0; i < TELNET_MAX_SRV_CLIENTS; i++)
    {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected())
      {
        if(serverClients[i]) 
        {
          Log.info("Lost connection to client %d [%s]\r\n", i, serverClients[i].remoteIP().toString().c_str());
          serverClients[i].stop();
        }
        if (serverClients[i] = server.available())
        {
          Log.info("New Telnet connection from client %d [%s]\r\n", i, serverClients[i].remoteIP().toString().c_str());
        }
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }  
  for(i = 0; i < TELNET_MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if(serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) {
          char inChar = serverClients[i].read();
          Log.verbose("Telnet: Received ASCII code %d", inChar);

          if ((int)inChar == 3) {
            clearBuffer(i);
            continue;
          }
          
          if (inChar == '\r' || inChar == '\n') {     // Check for the terminator (default '\r') meaning end of command
            char cmd[(buffer[i].length() + 1)];
            buffer[i].toCharArray(cmd, buffer[i].length() + 1);  
            if (0 == strncmp(cmd, "QUIT", 4)) {
              Log.info("Client %d [%s] closed connection by QUIT command\r\n", i, serverClients[i].remoteIP().toString().c_str());
              serverClients[i].stop();
            } else {            
              respond(i, dispatcher.processBuffer(cmd));
            }
            clearBuffer(i);
          }
          else if (isprint(inChar)) {     // Only printable characters into the buffer
            buffer[i] += inChar;
          }
        }  
      }
    }
  }  
}
void Telnet::setup() 
{  
  server.begin();
  server.setNoDelay(true);  
  for (uint8_t i = 0; i < TELNET_MAX_SRV_CLIENTS; i++) {
    // Buffer leeren
    buffer[i] = "";
  }
}
/*
 * Clear the input buffer.
 */
void Telnet::clearBuffer(uint8_t i) {
  buffer[i] = "";
}

void Telnet::respond(uint8_t client, String response) {
  if (serverClients[client] && serverClients[client].connected()){
    unsigned int strLength = response.length() + 1;
    char buf[strLength];
    response.toCharArray(buf, strLength);
    serverClients[client].write(buf, strLength);
  }
}

Telnet telnet = Telnet();
