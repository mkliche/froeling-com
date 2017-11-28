#include "AbstractJsonDataBroker.h"
#include "Logging.h"

WiFiClient AbstractJsonDataBroker::espClient = WiFiClient();

void AbstractJsonDataBroker::registerJsonDataProvider(std::function<void(JsonObject&)> dataProvider)
{
  JsonDataProvider* provider = new JsonDataProvider();
  provider->interval = getInterval() * 1000;
  provider->toJSON = dataProvider;
  
  providers = (JsonDataProvider**)realloc(providers, ++providerCount * sizeof(JsonDataProvider*));
  if (NULL == providers)
  {
    Log.error("Could not allocate memory for JSON data provider\r\n");

    return;
  }

  providers[providerCount - 1] = provider;

  Log.debug("Registered JSON data provider\r\n");
}

