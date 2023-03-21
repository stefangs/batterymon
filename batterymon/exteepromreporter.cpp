#include <Arduino.h>
#include <EEPROM.h>
// #include <AT24C256.h> // dantudose
#include "exteepromreporter.h"
#include "serialreporter.h"

#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME (60000)

struct ExtSample {
  unsigned int loadedVoltage : 11;
  unsigned int unloadedVoltage : 11;
} extSample;

void
ExtEEPromReporter::reportResume(int startVoltage) {
  nextSample = millis() + SAMPLE_TIME;
  slot = 0;
  EEPROM.get(0, extSample);
  // Find next free slot (or the last)
  while (((slot + 2) * sizeof(extSample) < EEPROM.length()) && extSample.unloadedVoltage != 0) {
    slot++;
    EEPROM.get(slot * sizeof(extSample), extSample);
  }
}

void
ExtEEPromReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  slot = 0;
  nextSample = millis();
  reportSample(0, loadVoltage, startVoltage, 0, 0);
}

void
ExtEEPromReporter::reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  if (abs(millis() - nextSample) < 30000) {
    nextSample += SAMPLE_TIME;
    extSample.loadedVoltage = loadedVoltage;
    extSample.unloadedVoltage = unloadedVoltage;
    EEPROM.put(slot * sizeof(extSample), extSample);
    if ((slot + 2) * sizeof(extSample) < EEPROM.length()) {
      slot++;      
    }
    extSample.unloadedVoltage = 0;
    EEPROM.put(slot * sizeof(extSample), extSample);
  }
}

void
ExtEEPromReporter::reportEnd(int mAh) {
}

void
ExtEEPromReporter::reportWaiting() {
}

void 
ExtEEPromReporter::printReport() {
  int readSlot = 0;
  long time = 0;
  SerialReporter reporter;
  int maxLoadedVoltage = 0;
  int minLoadedVoltage = 2000;
  int maxUnloadedVoltage = 0;
  int minUnloadedVoltage = 2000;
  int maxCurrent = 0;
  int minCurrent = 2000;
  long mAMinutes = 0;
  double joule = 0;
  
  Serial.println("*Start report*");
  EEPROM.get(0, extSample);
  while (extSample.unloadedVoltage != 0) {
    int current = extSample.loadedVoltage * 10 / 33;
    maxLoadedVoltage = max(maxLoadedVoltage, extSample.loadedVoltage);
    minLoadedVoltage = min(minLoadedVoltage, extSample.loadedVoltage);
    maxUnloadedVoltage = max(maxUnloadedVoltage, extSample.unloadedVoltage);
    minUnloadedVoltage = min(minUnloadedVoltage, extSample.unloadedVoltage);
    maxCurrent = max(maxCurrent, current);
    minCurrent = min(minCurrent, current);
    mAMinutes += current;
    joule += (extSample.loadedVoltage * (double)current * 60.0)  / 1000000.0;
    reporter.reportextSample(time, sample.loadedVoltage, sample.unloadedVoltage, current, 0);
    time += SAMPLE_TIME;
    readSlot += sizeof(Sample);
    EEPROM.get(readSlot, sample);
  }
  Serial.print("Max loaded voltage: ");
  Serial.print(maxLoadedVoltage);
  Serial.println(" mV");

  Serial.print("Min loaded voltage: ");
  Serial.print(minLoadedVoltage);
  Serial.println(" mV");

  Serial.print("Max unloaded voltage: ");
  Serial.print(maxUnloadedVoltage);
  Serial.println(" mV");

  Serial.print("Min unloaded voltage: ");
  Serial.print(minUnloadedVoltage);
  Serial.println(" mV");

  Serial.print("Max current: ");
  Serial.print(maxCurrent);
  Serial.println(" mA");
  
  Serial.print("Min current: ");
  Serial.print(minCurrent);
  Serial.println(" mA");

  Serial.print("Charge: ");
  Serial.print(mAMinutes / 60);
  Serial.println(" mAh");

  Serial.print("Energy: ");
  Serial.print(joule);
  Serial.println(" J");

  Serial.println("*End report*");
}
