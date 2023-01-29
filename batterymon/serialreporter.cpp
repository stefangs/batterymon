
#include <Arduino.h>
#include "serialreporter.h"

#define MINUTE_IN_MS (60000)

void
SerialReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  Serial.println("*Starting discharge*");
  Serial.print("Start voltage: ");
  Serial.println(startVoltage);
  Serial.print("Load voltage: ");
  Serial.println(loadVoltage);
  Serial.print("Current: ");
  Serial.println(current);
}

void
SerialReporter::reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  Serial.print(timeMs / MINUTE_IN_MS);
  Serial.print(',');
  Serial.print(loadedVoltage);
  Serial.print(',');
  Serial.print(unloadedVoltage);
  Serial.print(',');
  Serial.print(current);
  Serial.print(',');
  Serial.println(mAh);
}

void
SerialReporter::reportEnd() {
  Serial.println("Discharing stopped");
}

void
SerialReporter::reportWaiting() {
  Serial.println("Waiting for battery");
}
