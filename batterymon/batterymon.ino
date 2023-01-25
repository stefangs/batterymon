

#include "slot.h"

#define MINUTE_IN_MS (60000)
#define LOAD_PAUSE (200)

Slot slot0(2, A0);

bool isDischarging = false;
long startTime = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (!isDischarging) {
    if (slot0.voltage() > 1100) {
      slot0.addLoad();
      delay(LOAD_PAUSE);
      isDischarging = true;
      startTime = millis();
    }
  } else {
    int loaded = slot0.voltage();
    slot0.removeLoad();
    delay(LOAD_PAUSE);
    int unloaded = slot0.voltage();
    Serial.print((millis() - startTime) / MINUTE_IN_MS);
    Serial.print(',');  
    Serial.print(loaded);
    Serial.print(',');
    Serial.println(unloaded);
    if ((unloaded < 850) || (loaded < 400)) {
      isDischarging = false;
    } else {
      slot0.addLoad();
      delay(10000 - LOAD_PAUSE);
    }
  }
}
