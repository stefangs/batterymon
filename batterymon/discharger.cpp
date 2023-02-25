
#include <Arduino.h>
#include "discharger.h"
#include "reporter.h"
#include "slot.h"

#define LOAD_PAUSE (200)
#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME_IN_MS (60000)

Discharger::Discharger(Slot& slot, Reporter& reporter) : state(initial), slot(slot), reporter(reporter) {
}

void Discharger::loop() {
  slot.loop();
  switch (state) {
    case initial:
      doInitial();
      break;
    case idle:
      doIdle();
      break;
    case discharging:
      doDischarge();
      break;
    case ended:
      doEnded();
      break;
  }
}

inline bool isBatteryPresent(int voltage) {
  return voltage > 1100;
}

void
Discharger::doInitial() {
  if (isBatteryPresent(slot.voltage())) {
    // Battery already in, resume dischare
    reporter.reportResume(slot.voltage());
    nextSampleTime = millis() + SAMPLE_TIME_IN_MS;
    slot.addLoad();
    delay(LOAD_PAUSE);
    int loadVoltage = slot.voltage();
    int loadCurrent = loadVoltage * 10 / 33;
    state = discharging;
    startTime = millis();
    mA_Minutes = 0;
    lastVoltage = 0;
    numberOfLargeDrops = 0;
    nextSampleTime = startTime + SAMPLE_TIME_IN_MS;
    slot.getGreenLED().off();
    slot.getRedLED().blinkOn(400, 50);
    slot.getRedLED().blinkCount(loadCurrent/100);
  } else {
    state = idle;
  }
}

void
Discharger::doIdle() {
  slot.getGreenLED().on();
  if (isBatteryPresent(slot.voltage())) {
    delay(LOAD_PAUSE);
    int idleVoltage = slot.voltage();
    slot.addLoad();
    delay(LOAD_PAUSE);
    int loadVoltage = slot.voltage();
    int loadCurrent = loadVoltage * 10 / 33;
    reporter.reportStart(idleVoltage, loadVoltage, loadCurrent);
    state = discharging;
    startTime = millis();
    mA_Minutes = 0;
    lastVoltage = 0;
    numberOfLargeDrops = 0;
    nextSampleTime = startTime + SAMPLE_TIME_IN_MS;
    slot.getGreenLED().off();
    slot.getRedLED().blinkOn(400, 50);
    slot.getRedLED().blinkCount(loadCurrent/100);
  }
}

inline bool isBatteryRemoved(int voltage) {
  return voltage < 100;
}

inline bool isTimeForNextSample(long nextSampleTime) {
  return millis() > nextSampleTime;
}

bool 
Discharger::isVoltageDroppingRapidly(int loadVoltage) {
  int drop = lastVoltage - loadVoltage;
  lastVoltage = loadVoltage;
  if (drop > 15) {
    numberOfLargeDrops++;
  } else {
    numberOfLargeDrops = 0;
  }
  return (numberOfLargeDrops >= 3) && (loadVoltage < 1000);
}

inline bool isUnloadedBelowMinimum(int unloadedVoltage) {
  return unloadedVoltage < 850;
}
 
void
Discharger::doDischarge() {
  int loaded = slot.voltage();
  if (isBatteryRemoved(loaded)) {
    slot.removeLoad();
    reporter.reportEnd(mA_Minutes / 60);
    state = ended;
  } else if (isTimeForNextSample(nextSampleTime)) {
    int current = loaded * 10 / 33;
    slot.getRedLED().blinkCount(current/100);
    mA_Minutes += current;
    slot.removeLoad();
    delay(LOAD_PAUSE);
    int unloaded = slot.voltage();
    reporter.reportSample(millis() - startTime, loaded, unloaded, current, mA_Minutes / 60);
    nextSampleTime += SAMPLE_TIME_IN_MS;
    if (isUnloadedBelowMinimum(unloaded) || isVoltageDroppingRapidly(loaded)) {
      reporter.reportEnd(mA_Minutes / 60);
      state = ended;
    } else {
      slot.addLoad();
    }
  }
}

void
Discharger::doEnded() {
  slot.getGreenLED().on();
  slot.getRedLED().off();
  if (slot.voltage() < 10) {
    delay(500);
    state = idle;
    Serial.println("Waiting for battery");
  }
}
