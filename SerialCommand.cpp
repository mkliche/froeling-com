/**
 * SerialCommand - A Wiring/Arduino library to tokenize and parse commands
 * received over a serial port.
 * 
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 * 
 * Version 20120522
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SerialCommand.h"

/**
 * Constructor makes sure some things are set.
 */
SerialCommand::SerialCommand()
  : commandList(NULL),
    commandCount(0),
    defaultHandler(NULL),
    term('\n'),           // default terminator for commands, newline character
    last(NULL)
{
  strcpy(delim, " "); // strtok_r needs a null-terminated string
  clearBuffer();
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void SerialCommand::addCommand(const char *command, String (*callback)()) {
  
  Log.debug("Adding command (%d): %s\r\n", commandCount, command);

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIALCOMMAND_MAXCOMMANDLENGTH);
  commandList[commandCount].callback = callback;
  commandCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void SerialCommand::setDefaultHandler(String (*callback)(const char *)) {
  defaultHandler = callback;
}

String SerialCommand::processBuffer(char* buffer) {
  String returnValue = "";
  char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
  if (command != NULL) {
    boolean matched = false;
    for (int i = 0; i < commandCount; i++) {

      char* commandAtIndex = getCommandAtIndex(i);

      if (!commandAtIndex) {
        continue;
      }

      Log.debug("Comparing [%s] to [%s]\r\n", command, commandAtIndex);
      
      // Compare the found command against the list of known commands for a match
      if (strncmp(command, commandAtIndex, SERIALCOMMAND_MAXCOMMANDLENGTH) == 0) {
        
        Log.debug("Matched command: %s\r\n", command);

        // Execute the stored handler function for the command
        returnValue = execute(i);
        matched = true;
        break;
      }
    }
    if (!matched && (defaultHandler != NULL)) {
      returnValue = executeDefault(command);
    }
  }

  return returnValue;
}

String SerialCommand::execute(uint8_t index) {
  Log.verbose("Executing command %d\r\n", index);
  return (*commandList[index].callback)();
}

String SerialCommand::executeDefault(char* command) {
  Log.verbose("Executing default command\r\n");
  return (*defaultHandler)(command);
}

char* SerialCommand::getCommandAtIndex(uint8_t index) {
  return commandList[index].command;
}

/*
 * Clear the input buffer.
 */
void SerialCommand::clearBuffer() {
  buffer[0] = '\0';
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *SerialCommand::next() {
  return strtok_r(NULL, delim, &last);
}
