#include <Arduino.h>
#include <EEPROM.h>
#include "eepromreporter.h"

#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME (60000 * 2)

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
    slot++;
    sample.unloadedVoltage = 0;
    EEPROM.put(slot * sizeof(Sample), sample);
  }
}

void
EEPromReporter::reportEnd() {
}

void
EEPromReporter::reportWaiting() {
}
