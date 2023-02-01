#include <Arduino.h>
#include <EEPROM.h>
#include "eepromreporter.h"
#include "serialreporter.h"

#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME (60000)

struct Sample {
  unsigned int loadedVoltage : 11;
  unsigned int unloadedVoltage : 11;
} sample;

void
EEPromReporter::reportStart(int startVoltage, int loadVoltage, int current) {
  slot = 0;
  nextSample = millis();
  reportSample(0, loadVoltage, startVoltage, 0, 0);
}

void
EEPromReporter::reportSample(long unsigned int timeMs, int loadedVoltage, int unloadedVoltage, int current, int mAh) {
  if (abs(millis() - nextSample) < 30000) {
    nextSample += SAMPLE_TIME;
    sample.loadedVoltage = loadedVoltage;
    sample.unloadedVoltage = unloadedVoltage;
    EEPROM.put(slot * sizeof(Sample), sample);
    if ((slot + 2) * sizeof(Sample) < EEPROM.length()) {
      slot++;      
    }
    sample.unloadedVoltage = 0;
    EEPROM.put(slot * sizeof(Sample), sample);
  }
}

void
EEPromReporter::reportEnd(int mAh) {
}

void
EEPromReporter::reportWaiting() {
}

void 
EEPromReporter::printReport() {
  int readSlot = 0;
  long time = 0;
  SerialReporter reporter;
  EEPROM.get(0, sample);
  Serial.println("*Start report*");
  while (sample.unloadedVoltage != 0) {
    reporter.reportSample(time, sample.loadedVoltage, sample.unloadedVoltage, sample.loadedVoltage * 10 / 33, 0);
    time += SAMPLE_TIME;
    readSlot += sizeof(Sample);
    EEPROM.get(readSlot, sample);
  }
  Serial.println("*End report*");
}
