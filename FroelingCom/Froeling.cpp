#include "Froeling.h"
#include <functional>
#include "HttpDispatcher.h"
#include "MQTT.h"
#include "ThingSpeakBroker.h"
#include <functional>

void Froeling::setup()
{
  httpDispatcher.registerJsonGetHandler(getKey(), std::bind(&Froeling::toJSON, this, std::placeholders::_1));
  mqtt.registerJsonDataProvider(std::bind(&Froeling::toJSON, this, std::placeholders::_1));
  tsBroker.registerJsonDataProvider(std::bind(&Froeling::toJSON, this, std::placeholders::_1));
  FroeCom_Init();
}

void Froeling::loop()
{
  FroeComSTM();
}

void Froeling::toJSON(JsonObject& json)
{
//  json["tCurrentMainState"] = tFroeComStmData.tCurrentMainState;
//  json["tCurrentSubState"] = tFroeComStmData.tCurrentSubState;
//  json["cReceiveBuffer"] = tFroeComStmData.cReceiveBuffer;
//  json["iInBuffPointer"] = tFroeComStmData.iInBuffPointer;
//  //json["cRbSendFlag"] = tFroeComStmData.cRbSendFlag;
//  json["iMsgLength"] = tFroeComStmData.iMsgLength;
//  json["glsUserID"] = FROELING_USER_ID;
//  json["iLastFroelingTimestamp"] = tFroeComStmData.iLastFroelingTimestamp;
//  json["ulLastSystemTimestamp"] = tFroeComStmData.ulLastSystemTimestamp;



//  json["status"] =  froeling_proc_value_array[0];
//  json["zustand"] =  froeling_proc_value_array[1];
//  json["rost"] =  froeling_proc_value_array[2];
//  json["kesseltemp"] =  froeling_proc_value_array[3];
//  json["abgastemp"] =  froeling_proc_value_array[4];
//  json["abgas_sw"] =  froeling_proc_value_array[5];
//  json["kess_stell_gr"] =  froeling_proc_value_array[6];
//  json["saugzug"] =  froeling_proc_value_array[7];
//  json["prim_luft"] =  froeling_proc_value_array[8];
//  json["prim_kl_pos"] =  froeling_proc_value_array[9];
//  json["rest_o2"] =  froeling_proc_value_array[10];
//  json["o2_regler"] =  froeling_proc_value_array[11];
//  json["sek_luft"] =  froeling_proc_value_array[12];
//  json["sek_kl_pos"] =  froeling_proc_value_array[13];
//  json["puffer_oben"] =  froeling_proc_value_array[14];
//  json["puffer_unten"] =  froeling_proc_value_array[15];
//  json["puffer_pu"] =  froeling_proc_value_array[16];
//  json["boilertemp"] =  froeling_proc_value_array[17];
//  json["oelkessel"] =  froeling_proc_value_array[18];
//  json["aussentemp"] =  froeling_proc_value_array[19];
//  json["vorlauftemp_1sw"] =  froeling_proc_value_array[20];
//  json["vorlauftemp_1"] =  froeling_proc_value_array[21];
//  json["kty6_h2"] =  froeling_proc_value_array[22];
//  json["kty7_h2"] =  froeling_proc_value_array[23];
//  json["laufzeit"] =  froeling_proc_value_array[24];
//  json["f_halt"] =  froeling_proc_value_array[25];
//  json["boardtemp"] = froeling_proc_value_array[26];

  /**
   * Diese Werte müssen unbedingt an die eigene Heizung angepasst werden!
   */
  json["status"] =  String(froeling_proc_value_array[0]);
  json["zustand"] =  String(froeling_proc_value_array[1]);
  json["rost"] =  String(froeling_proc_value_array[2]);
  json["kesseltemp"] =  String(froeling_proc_value_array[3]);
  json["abgastemp"] =  String(froeling_proc_value_array[4]);
  json["abgas_sw"] =  String(froeling_proc_value_array[5]);
  json["kess_stell_gr"] =  String(froeling_proc_value_array[6]);
  json["saugzug"] =  String(froeling_proc_value_array[7]);
  json["prim_luft"] =  String(froeling_proc_value_array[8]);
  json["prim_kl_pos"] =  String(froeling_proc_value_array[9]);
  json["rest_o2"] =  String(froeling_proc_value_array[10]);
  json["o2_regler"] =  String(froeling_proc_value_array[11]);
  json["sek_luft"] =  String(froeling_proc_value_array[12]);
  json["sek_kl_pos"] =  String(froeling_proc_value_array[13]);
  json["puffer_oben"] =  String(froeling_proc_value_array[14]);
  json["puffer_unten"] =  String(froeling_proc_value_array[15]);
  json["puffer_pu"] =  String(froeling_proc_value_array[16]);
  json["boilertemp"] =  String(froeling_proc_value_array[17]);
  json["oelkessel"] =  String(froeling_proc_value_array[18]);
  json["aussentemp"] =  String(froeling_proc_value_array[19]);
  json["vorlauftemp_1sw"] =  String(froeling_proc_value_array[20]);
  json["vorlauftemp_1"] =  String(froeling_proc_value_array[21]);
  json["kty6_h2"] =  String(froeling_proc_value_array[22]);
  json["kty7_h2"] =  String(froeling_proc_value_array[23]);
  json["laufzeit"] =  String(froeling_proc_value_array[24]);
  json["f_halt"] =  String(froeling_proc_value_array[25]);
  json["boardtemp"] = String(froeling_proc_value_array[26]);  
}

void Froeling::fromJSON(JsonObject& json)
{
  // Do nothing
}

char* Froeling::getKey()
{
  return "froeling";
}

Froeling froeling = Froeling();

