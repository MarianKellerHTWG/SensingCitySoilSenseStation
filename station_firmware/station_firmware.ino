#include <MKRNB.h>
#include <ArduinoJson.h>
#include <ArduinoLowPower.h>
#include <RTCZero.h>


#include "smt100.h"
#include "smt50.h"

#include "config.h"
#include "arduino_secrets.h" 

const char PINNUMBER[] = SECRET_PINNUMBER;
const char APN[] = "em";

NB nbAccess(true);
NBScanner scannerNetworks;
NBMqtt mqtt;

double round2(double value) {
  return (int)(value * 100 + 0.5) / 100.0;
}

float get_supply_voltage() {
  // change ADC resolution to 12 bits
  analogReadResolution(12);
  int raw_val = analogRead(VOLTAGE_SENSE_PIN);
  float voltage = ((float) raw_val / (float) ADC_MAX) * ADC_REF_VOLTAGE;
  return voltage * VOLTAGE_SENSE_RATIO;
}

void setup() {
  // Serial Console
  //Serial.begin(115200);

  // RS485 Bus
  Serial1.begin(9600);

  pinMode(SENSOR2_TEMPERATURE_PIN, INPUT);
  pinMode(SENSOR2_MOISTURE_PIN, INPUT);
  pinMode(VOLTAGE_SENSE_PIN, INPUT);


  float curr_supply_voltage = get_supply_voltage();

  // Don't consume any more power if supply voltage is too low.
  // Supply voltage under 1 V with running microcontroller indicates
  // voltage sense failure, so that can be ignored.
  if (curr_supply_voltage > 1 && curr_supply_voltage < 7) {
    LowPower.deepSleep(3600000);
    NVIC_SystemReset();      // processor software reset
  }

  //while (!Serial);

  //Serial.println("Initializing SARA MQTT");

  bool connected = false;

  // start GSM connection
  while (!connected) {
    if (nbAccess.begin(PINNUMBER, "em") == NB_READY) {
      connected = true;
    } else {
      //Serial.println("Not connected");
      nbAccess.shutdown();
      delay(1000);
    }
  }

  //Serial.println("NB initialized");

  mqtt.setMQTTClientID(IMEI);
  mqtt.setMQTTPort(1883);
  mqtt.setMQTTBrokerURL("vm1.mondix.de");
  //mqtt.setMQTTBrokerIP(MQTT_BROKER_IP, 1883); 
  mqtt.setMQTTUserPassword(MQTT_USER, MQTT_PASSWORD); // User, Password
  mqtt.setMQTTBrokerConnect(true);
}

char json_out[JSON_BUFFER_SIZE];

void loop() {

  StaticJsonDocument<400> doc;

  JsonObject station_0 = doc["station"].createNestedObject();
  station_0["stationId"] = STATION_ID;
  station_0["batV"] = round2(get_supply_voltage());
  
  JsonObject station_0_sensor0 = station_0.createNestedObject("sensor0");
  station_0_sensor0["typ"] = "SMT100";
  station_0_sensor0["temp"] = round2(smt100_get_temperature(1));
  station_0_sensor0["e"] = smt100_get_permittivity(1);
  station_0_sensor0["cnts"] = smt100_get_counts(1);
  
  JsonObject station_0_sensor1 = station_0.createNestedObject("sensor1");
  station_0_sensor1["typ"] = "SMT100";
  station_0_sensor1["temp"] = round2(smt100_get_temperature(2));
  station_0_sensor1["e"] = smt100_get_permittivity(2);
  station_0_sensor1["cnts"] = smt100_get_counts(2);
  
  JsonObject station_0_sensor2 = station_0.createNestedObject("sensor2");
  station_0_sensor2["typ"] = "SMT50";
  station_0_sensor2["temp"] = round2(smt50_get_temperature());
  station_0_sensor2["v"] = round2(smt50_get_voltage());

  serializeJson(doc, json_out);
  
  //Serial.println(json_out);
  //delay(100);
  mqtt.sendMQTTMsg("/sensors", json_out, true);

  String mqtt_error = mqtt.getMQTTerror();
  //Serial.print("MQTT Error (See page 408 of the u-blox AT Manual): ");
  //Serial.println(mqtt_error);

  mqtt.setMQTTBrokerConnect(false);
  
  nbAccess.shutdown();
  
  LowPower.deepSleep(3600000);
  
  NVIC_SystemReset();      // processor software reset
}
