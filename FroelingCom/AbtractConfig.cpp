#include "AbstractConfig.h"
#include "Logging.h"

char* AbstractConfig::setStringProperty(char* propPtr, const char* value, const char* propName)
{
  if (NULL == value) {
    return propPtr;
  }
  unsigned int len = strlen(value);
  char* tmpPtr = (char*)realloc(propPtr, (len + 1) * sizeof(char));
  if (NULL == tmpPtr) {
    Log.error("Failed realloc %s[%s] for value \"%s\"\r\n", getKey(), propName, value);
    return propPtr;
  }
  strcpy(tmpPtr, value);
  Log.debug("Set %s[%s] to \"%s\"\r\n", getKey(), propName, propPtr);

  free(propPtr);
  propPtr = tmpPtr;

  return propPtr;
}

