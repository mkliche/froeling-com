#include "SerialCommandDispatcher.h"
#include "Logging.h"

SerialCommandDispatcher::SerialCommandDispatcher() : SerialCommand() {
  
}

void SerialCommandDispatcher::setup() {
  addCommand("HELLO", &SerialCommandDispatcher::sayHello);
}

void SerialCommandDispatcher::loop() {

}

String SerialCommandDispatcher::sayHello() {
 
  char *arg;
  String ret;
  arg = next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    ret = "Hello " + String(arg) + "!\r\n";
  } else {
    ret = "Hello, whoever you are!\r\n";
  }

  return ret;
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void SerialCommandDispatcher::addCommand(const char *command, String (SerialCommandDispatcher::*callback)()) {
  
  Log.debug("Adding command (%d): %s\r\n", commandCount, command);

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].callback = callback;
  commandCount++;
}

char* SerialCommandDispatcher::getCommandAtIndex(uint8_t index) {
  return commandList[index].command;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void SerialCommandDispatcher::setDefaultHandler(String (SerialCommandDispatcher::*callback)(const char *)) {
  defaultHandler = callback;
}


String SerialCommandDispatcher::execute(uint8_t index) {  
  Log.verbose("Executing command %d\r\n", index);
  return (this->*commandList[index].callback)();
}

String SerialCommandDispatcher::executeDefault(char* command) {
  Log.verbose("Executing default command\r\n");
  return (this->*defaultHandler)(command);
}

uint8_t SerialCommandDispatcher::parse0toMax(uint8_t max) {
  char* arg = next();
  uint8_t c = 0;
  if(NULL != arg) {
    c = atoi(arg);
    if(c < 0) c = 0;
    if(c > max) c = max;
    return c;
  }
}

SerialCommandDispatcher dispatcher = SerialCommandDispatcher();
