#include "Logging.h"
#include <stdio.h>
#include "telnet.h"

int vasprintf(char** strp, const char* fmt, va_list ap) {
  va_list ap2;
  va_copy(ap2, ap);
  char tmp[1];
  int size = vsnprintf(tmp, 1, fmt, ap2);
  if (size <= 0) return size;
  va_end(ap2);
  size += 1;
  *strp = (char*)malloc(size * sizeof(char));
  return vsnprintf(*strp, size, fmt, ap);
}

Logging::Logging()
{
}

void Logging::init(int level, long baud) {
  _level = constrain(level, LOG_LEVEL_NOOUTPUT, LOG_LEVEL_VERBOSE);
  _baud = baud;
  if (_serial)
  {
    Serial.begin(_baud);
  }
}

void Logging::error(char* msg, ...) {
  if (LOG_LEVEL_ERRORS <= _level) {
    print ("ERROR: ");
    va_list args;
    va_start(args, msg);
    print(msg, args);
  }
}


void Logging::info(char* msg, ...) {
  if (LOG_LEVEL_INFOS <= _level) {
    va_list args;
    va_start(args, msg);
    print(msg, args);
  }
}

void Logging::debug(char* msg, ...) {
  if (LOG_LEVEL_DEBUG <= _level) {
    va_list args;
    va_start(args, msg);
    print(msg, args);
  }
}


void Logging::verbose(char* msg, ...) {
  if (LOG_LEVEL_VERBOSE <= _level) {
    va_list args;
    va_start(args, msg);
    print(msg, args);
  }
}


void Logging::print(const char *format) {
  if (_serial)
  {
    Serial.print(*format);
  }
  telnet.send(*format);
}

void Logging::print(const char *format, va_list args) {

  unsigned int formatLength = strlen(format) + 64; // Reserve

  char* buf;
  vasprintf(&buf, format, args);
  if (_serial)
  {
    Serial.print(buf);
  }
  telnet.send(buf);

  free(buf);
}

Logging Log = Logging();
