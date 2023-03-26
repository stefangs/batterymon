#include <Arduino.h>
#include <AT24C256.h> // dantudose
#include "exteepromreporter.h"
#include "serialreporter.h"

#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME (60000)
#define MARK (0xAABBCCD0)
#define REPORT_START (100)
#define REPORT_SIZE (4096)
#define SLOTS_PER_REPORT (REPORT_SIZE/sizeof(ExtSample))

AT24C256 eeprom(0x50);

struct ExtSample {
  unsigned int loadedVoltage : 11;
  unsigned int unloadedVoltage : 11;
} extSample;

struct ExtHeader {
  unsigned long mark;
  int lastReport;
} header;

ExtEEPromReporter::ExtEEPromReporter() {
  state = ok;
  eeprom.read(0, (uint8_t *)&header, sizeof(ExtHeader));
  if (header.mark != MARK) {
    state = initialized;
    header.mark = MARK;
    header.lastReport = 0;
    eeprom.write(0, (uint8_t*)&header, sizeof(ExtHeader));
    slot = 0;
    extSample.unloadedVoltage = 0;
    writeSample(&extSample);  
  }
  eeprom.read(0, (uint8_t *)&header, sizeof(ExtHeader));
  if (header.mark != MARK) {
    state = error;
  }
}

void printState(EEPromState state) {
  if (state == initialized) {
    Serial.println("No mark, resetting eeprom");
  } else if (state == error) {
    Serial.println("*** Error, could not write to eeprom ***");
  }
}

void
ExtEEPromReporter::writeSample(ExtSample* sample){
  eeprom.write(REPORT_START + slot * sizeof(ExtSample), (uint8_t*)sample, sizeof(ExtSample));
}

void
ExtEEPromReporter::readSample(ExtSample* sample){
  eeprom.read(REPORT_START + slot * sizeof(ExtSample), (uint8_t*)sample, sizeof(ExtSample));
}

void
ExtEEPromReporter::reportResume(int startVoltage) {
  printState(state);
  nextSample = millis() + SAMPLE_TIME;
  slot = 0;
  readSample(&extSample);
  // Find next free slot (or the last)
  while ((slot + 2 > SLOTS_PER_REPORT) && extSample.unloadedVoltage != 0) {
    slot++;
    readSample(&extSample);
  }
}

void
ExtEEPromReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  printState(state);
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
    writeSample(&extSample);
    if (slot + 2 > SLOTS_PER_REPORT)  {
      slot++;      
    }
    extSample.unloadedVoltage = 0;
    writeSample(&extSample);
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
  int currentSlot = slot;
  slot = 0;
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
  readSample(&extSample);
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
    reporter.reportSample(time, extSample.loadedVoltage, extSample.unloadedVoltage, current, 0);
    time += SAMPLE_TIME;
    slot++;
    readSample(&extSample);
  }
  slot = currentSlot;
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
