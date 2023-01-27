

#include "discharger.h"

#define MINUTE_IN_MS (60000)
#define LOAD_PAUSE (200)

Slot slot0(2, A0);
Discharger discharger0(slot0);

bool isDischarging = false;
long startTime = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  discharger0.loop();
}
