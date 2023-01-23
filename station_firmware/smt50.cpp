#include "smt50.h"
#include "config.h"

float smt50_get_temperature() {
  // change ADC resolution to 12 bits
  analogReadResolution(12);
  int raw_val = analogRead(SENSOR2_TEMPERATURE_PIN);
  float voltage = ((float) raw_val / (float) ADC_MAX) * ADC_REF_VOLTAGE;
  float temperature = (voltage - 0.5) * 100;
  return temperature;
}
float smt50_get_voltage() {
  // change ADC resolution to 12 bits
  analogReadResolution(12);
  int raw_val = analogRead(SENSOR2_MOISTURE_PIN);
  float voltage = ((float) raw_val / (float) ADC_MAX) * ADC_REF_VOLTAGE;
  return voltage;
}
