#include "smt100.h"
float smt100_get_temperature(int address) {
  String answer = "-300";
  Serial1.print("GetTemperature!");
  Serial1.println(address, DEC);
  
  delay(500);
  
  if (Serial1.available() > 0) {
    answer = Serial1.readStringUntil('\n');
  } else {
    answer = "-300";
  }
  return answer.toFloat();
}

float smt100_get_permittivity(int address) {
  String answer = "-1";
  Serial1.print("GetPermittivity!");
  Serial1.println(address, DEC);
  
  delay(500);
  
  if (Serial1.available() > 0) {
    answer = Serial1.readStringUntil('\n');
  } else {
    answer = "-1";
  }
  return answer.toFloat();
}

float smt100_get_counts(int address) {
  String answer = "-1";
  Serial1.print("GetCounts!");
  Serial1.println(address, DEC);
  
  delay(500);
  
  if (Serial1.available() > 0) {
    answer = Serial1.readStringUntil('\n');
  } else {
    answer = "-1";
  }
  return answer.toFloat();
}
