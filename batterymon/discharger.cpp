
#include "discharger.h"

void Discharger::Discharger(Slot& slot) : state(idle), slot(slot){
}

void Discharger::loop() {
  switch (state) {
    case idle:
      idle();
      break;
    case discharging:
      discharge();
      break;
  }
}

void
Discharger::idle(){
    if (slot0.voltage() > 1100) {
      slot0.addLoad();
      delay(LOAD_PAUSE);
      isDischarging = true;
      startTime = millis();
    }
}

void
Discharger::discharge(){
}
