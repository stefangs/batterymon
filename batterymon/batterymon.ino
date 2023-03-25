
#include "discharger.h"
#include "slot.h"
#include "serialreporter.h"
#include "eepromreporter.h"
#include "doublereporter.h"
#include "exteepromreporter.h"

Slot slot0(2, A0, 3, 4);
SerialReporter sreporter;
EEPromReporter  ereporter;
DoubleReporter dreporter(sreporter, ereporter);
Discharger discharger0(slot0, dreporter);


void setup() {
  Serial.begin(115200);
  ereporter.printReport();

  ExtEEPromReporter extEeprom(0);
}

void loop() {
  discharger0.loop();
}
