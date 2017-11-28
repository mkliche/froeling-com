#ifndef SerialCommandDispatcher_h
#define SerialCommandDispatcher_h

#include "SerialCommand.h"

class SerialCommandDispatcher : public SerialCommand {
  public:
    SerialCommandDispatcher();
    void addCommand(const char *command, String(SerialCommandDispatcher::*callback)());  // Add a command to the processing dictionary.
    void setDefaultHandler(String (SerialCommandDispatcher::*callback)(const char *));   // A handler to call when no valid command received.
    void setup();
    void loop();
    String sayHello();

  protected:
    String execute(uint8_t index);
    String executeDefault(char* command);
    char* getCommandAtIndex(uint8_t index);

  private:
    // Command/handler dictionary
    struct SerialCommandCallback {
      char command[SERIALCOMMAND_MAXCOMMANDLENGTH + 1];
      String (SerialCommandDispatcher::*callback)();
    };
    SerialCommandCallback *commandList;   // Actual definition for command/handler array

    // Pointer to the default handler function
    String (SerialCommandDispatcher::*defaultHandler)(const char *);

    uint8_t parse0toMax(uint8_t max = 255);
};

extern SerialCommandDispatcher dispatcher;

#endif
