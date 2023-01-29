

#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"

Slot slot0(2, A0);
SerialReporter reporter;
Discharger discharger0(slot0, reporter);

void setup() {
  Serial.begin(115200);
}

void loop() {
  discharger0.loop();
}
