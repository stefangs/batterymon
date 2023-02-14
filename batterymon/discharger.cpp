
#include <Arduino.h>
#include "discharger.h"
#include "reporter.h"
#include "slot.h"

#define LOAD_PAUSE (200)
#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME_IN_MS (60000)

Discharger::Discharger(Slot& slot, Reporter& reporter) : state(idle), slot(slot), reporter(reporter) {
}

void Discharger::loop() {
  slot.loop();
  switch (state) {
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

void
Discharger::doIdle() {
  slot.getGreenLED().on();
  if (slot.voltage() > 1100) {
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
    nextSampleTime = startTime + SAMPLE_TIME_IN_MS;
    slot.getGreenLED().off();
    slot.getRedLED().blinkOn(400, 50);
    slot.getRedLED().blinkCount(loadCurrent/100 + 1);
  }
}

inline bool isBatteryRemoved(int voltage) {
  return voltage < 100;
}

inline bool isTimeForNextSample(long nextSampleTime) {
  return millis() > nextSampleTime;
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
    slot.getRedLED().blinkCount(current/100 + 1);
    mA_Minutes += current;
    slot.removeLoad();
    delay(LOAD_PAUSE);
    int unloaded = slot.voltage();
    reporter.reportSample(millis() - startTime, loaded, unloaded, current, mA_Minutes / 60);
    nextSampleTime = nextSampleTime + SAMPLE_TIME_IN_MS;
    if (unloaded < 850) {
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
