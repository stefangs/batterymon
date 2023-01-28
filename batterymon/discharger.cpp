
#include <Arduino.h>
#include "discharger.h"

#define LOAD_PAUSE (200)
#define MINUTE_IN_MS (60000)
#define SAMPLE_TIME_IN_MS (60000)

Discharger::Discharger(Slot& slot) : state(idle), slot(slot){
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
    Serial.println("*Starting discharge*");
    Serial.print("Start voltage: ");  
    Serial.println(idleVoltage);
    slot.addLoad();
    delay(LOAD_PAUSE);
    int loadVoltage = slot.voltage();
    Serial.print("Load voltage: ");  
    Serial.println(loadVoltage);
    Serial.print("Current: ");  
    Serial.println(loadVoltage * 10 / 33);
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
    Serial.println("Discharing stopped");
    state = ended;
  } else if (millis() > nextSampleTime) {
    int current = loaded * 10 / 33;
    mA_Minutes += current;
    slot.removeLoad();
    delay(LOAD_PAUSE);
    int unloaded = slot.voltage();
    Serial.print((millis() - startTime) / MINUTE_IN_MS);
    Serial.print(',');
    Serial.print(loaded);
    Serial.print(',');
    Serial.print(unloaded);
    Serial.print(',');
    Serial.print(current);
    Serial.print(',');
    Serial.println(mA_Minutes/60);
    nextSampleTime = nextSampleTime + SAMPLE_TIME_IN_MS;
    if (unloaded < 850) {
      Serial.println("Discharing stopped");
      state = ended;
    } else {
      slot.addLoad();
    }
  }
}

void
Discharger::doEnded(){
  if (slot.voltage() < 10) {
    state = idle;
    Serial.println("Waiting for battery");
  }
}
