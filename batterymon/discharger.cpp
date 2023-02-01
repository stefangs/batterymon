
#include <Arduino.h>
#include "discharger.h"
#include "reporter.h"
#include "slot.h"

#define LOAD_PAUSE (200)
#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME_IN_MS (60000)

Discharger::Discharger(Slot& slot, Reporter& reporter) : state(idle), slot(slot), reporter(reporter){
}

void Discharger::loop() {
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
Discharger::doIdle(){
  if (slot.voltage() > 1100) {
    delay(LOAD_PAUSE);
    int idleVoltage = slot.voltage();
    slot.addLoad();
    delay(LOAD_PAUSE);
    int loadVoltage = slot.voltage();
    reporter.reportStart(idleVoltage, loadVoltage, loadVoltage * 10 / 33);
    state = discharging;
    startTime = millis();
    mA_Minutes = 0;
    nextSampleTime = startTime + SAMPLE_TIME_IN_MS;
  }
}

void
Discharger::doDischarge(){
  int loaded = slot.voltage();
  if (loaded < 100) {
    int current = loaded * 10 / 33;
    mA_Minutes += current;
    slot.removeLoad();
    reporter.reportEnd(mA_Minutes / 60);
    state = ended;
  } else if (millis() > nextSampleTime) {
    int current = loaded * 10 / 33;
    mA_Minutes += current;
    slot.removeLoad();
    delay(LOAD_PAUSE);
    int unloaded = slot.voltage();
    reporter.reportSample(millis() - startTime, loaded, unloaded, current, mA_Minutes/60);
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
Discharger::doEnded(){
  if (slot.voltage() < 10) {
    delay(500);
    state = idle;
    Serial.println("Waiting for battery");
  }
}
